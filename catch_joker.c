/* catch joker */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "poker.h"

typedef struct search_s
{
    int player_no;
    int index;
    int card_num;
} SEARCH_T;

char    card_map[15][3] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
DECK_TP deck = NULL;

void dump_card(int index, int card, void *para)
{
    char color = 'U';
    switch (POKER_Color(card))
    {
        case POKER_COLOR_SPADE:
            color = 'B';
            break;
        case POKER_COLOR_HEART:
            color = 'H';
            break;
        case POKER_COLOR_DIAMOND:
            color = 'D';
            break;
        case POKER_COLOR_CLUB:
            color = 'C';
            break;
        case POKER_COLOR_JOKER:
            color = 'J';
            break;
    }
    printf("[%d]%c-%s\t", index, color, card_map[POKER_Num(card)]);
}

int throw_func(int index, int card, void *para)
{
    SEARCH_T *search = (SEARCH_T *)para;
    
    if (index == search->index) return POKER_ERR; /* skip the first search card */
    if (POKER_Num(card) == search->card_num) /* match the second search card */
    {
        /* throw the second search card */
        POKER_Throw_PlayerCard(deck, search->player_no, POKER_FROM_INDEX, index);
        return POKER_OK;
    }
    return POKER_NONE;
}

int search_func(int index, int card, void *para)
{
    SEARCH_T search;

    search.player_no = *(int *)para;
    search.index = index;
    search.card_num = POKER_Num(card);
    if (POKER_Search_PlayerPile(deck, search.player_no, throw_func, &search) == POKER_OK)
    {
        /* if match second card, throw the first card, and return for re-search others */
        POKER_Throw_PlayerCard(deck, search.player_no, POKER_FROM_INDEX, index);
        return POKER_OK;
    }
    return POKER_NONE;
}

int comp_func(int card1, int card2)
{
    if (POKER_Color(card1) == POKER_Color(card2))
    {
        if (POKER_Num(card1) == POKER_NUM_A) return -1;
        else if (POKER_Num(card2) == POKER_NUM_A) return 1;
        return card1 - card2;
    }
    return POKER_Color(card2) - POKER_Color(card1);
}

/* argv[1]: players */
int main(int argc, char **argv)
{
    int players = atoi(argv[1]);
    int player_no = 0;
    int player_next = 0;
    int rand_idx = 0;

    srand(time(NULL));

    /* create a deck of poker */
    deck = POKER_Create_Deck(players, 1);
    /* shuffle pile */
    POKER_Shuffle_LastPile(deck);

#if 1
    POKER_Dump_LastPile(deck, dump_card, NULL);
    printf("\n");
#endif

#if 0
    /* test for sorting */
    POKER_Sort_LastPile(deck, comp_func);
    POKER_Dump_LastPile(deck, dump_card, NULL);
    printf("\n");
#endif

    /* Deal cards */
    player_no = 0;
    while (POKER_Get_LastCardNum(deck) > 0)
    {
        if (++player_no > players) player_no = 1;
        if (POKER_Deal_Card(deck, POKER_FROM_TOP, 0, player_no) != POKER_OK)
        {
            printf("ERROR: deal card fail, %s\n", strerror(errno));
            return -1;
        }
    }

#ifdef TEST
    printf("\nFinish to deal cards\n");
    for (player_no = 1; player_no <= players; player_no++)
    {
        printf("Player %d\n", player_no);
        POKER_Dump_PlayerPile(deck, player_no, dump_card, NULL);
        printf("\n");
    }
#endif
    
    /* throw the same card no */
    printf("\nThrow twice of card for the same number\n");
    for (player_no = 1; player_no <= players; player_no++)
    {
        while (POKER_Search_PlayerPile(deck, player_no, search_func, &player_no) != POKER_NONE);
    }

#ifdef TEST
    for (player_no = 1; player_no <= players; player_no++)
    {
        printf("Player %d\n", player_no);
        POKER_Dump_PlayerPile(deck, player_no, dump_card, NULL);
        printf("\n");
    }
#endif

    player_no = 1;
    /* loop for trnasfer players' card until joker card left */
    while (POKER_Get_TrashCardNum(deck) < POKER_Get_TotalCardNum(deck) - 1)
    {
        printf("\nPress any key to turn next round ...");
        getc(stdin);
        /* skip the empty-handed player */
        while (POKER_Get_PlayerCardNum(deck, player_no) == 0)
            if (++player_no > players) player_no = 1;
            
        player_next = (player_no + 1 > players) ? 1 : (player_no + 1);
        /* skip the empty-handed player */
        while (POKER_Get_PlayerCardNum(deck, player_next) == 0)
            if (++player_next > players) player_next = 1;
            
        /* draw a card from next player */
        rand_idx = rand() % POKER_Get_PlayerCardNum(deck, player_next);
        printf("Player %d draws from %d ===> ", player_no, player_next);
        dump_card(rand_idx, POKER_Show_PlayerCard(deck, player_next, POKER_FROM_INDEX, rand_idx), NULL);
        POKER_Transfer_PlayerCard(deck, POKER_FROM_INDEX, rand_idx, player_next, player_no);

        /* dump */
        printf("\nPlayer %d: ", player_no);
        POKER_Dump_PlayerPile(deck, player_no, dump_card, NULL);
        printf("\nPlayer %d: ", player_next);
        POKER_Dump_PlayerPile(deck, player_next, dump_card, NULL);

        /* check the same card number and throw them */
        printf("\nPlayer %d throw twice of card for the same number", player_no);
        POKER_Search_PlayerPile(deck, player_no, search_func, &player_no);
        printf("\nPlayer %d Result: ", player_no);
        POKER_Dump_PlayerPile(deck, player_no, dump_card, NULL);

        /* next player */
        player_no = player_next;
    }

    /* check who has the last joker card */
    for (player_no = 1; player_no <= players; player_no++)
    {
        if (POKER_Get_PlayerCardNum(deck, player_no) > 0)
        {
            printf("\n\nPlayer %d lose the game !!\n", player_no);
            break;
        }
    }

    /* test for shuffle trash pile */
    POKER_Shuffle_TrashPile(deck);
#if 0
    POKER_Dump_LastPile(deck, dump_card, NULL);
    printf("\n");
#endif

    POKER_Delete_Deck(&deck);
    return 0;
}

