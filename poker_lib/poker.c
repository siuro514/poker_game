/* poker library */
#include "poker.h"

struct card_s
{
    int             card;
    struct card_s   *next;
    struct card_s   *prev;
};

struct pile_s
{
    struct card_s   *top;
    struct card_s   *bottom;
    int             card_num;
};

struct deck_s
{
    struct pile_s   last_pile;   /* last pile */
    struct pile_s   trash_pile;  /* trash pile */
    struct pile_s   *player;     /* players' pile */
    int             total_num;
    int             joker_num;
    int             player_num;
};

static void swap(int *card1, int *card2)
{
    int tmp = *card1;
    *card1 = *card2;
    *card2 = tmp;
}

static int insert_card(PILE_T *pile, CARD_T *card, int type)
{
    switch (type)
    {
        case POKER_FROM_TOP:
            if (pile->top == NULL)
            {
                pile->top = pile->bottom = card;
                card->prev = card->next = NULL;
            }
            else
            {
                card->next = pile->top;
                card->prev = NULL;
                pile->top->prev = card;
                pile->top = card;
            }
            break;
        case POKER_FROM_BOTTOM:
            if (pile->top == NULL)
            {
                pile->top = pile->bottom = card;
                card->prev = card->next = NULL;
            }
            else
            {
                card->prev = pile->bottom;
                card->next = NULL;
                pile->bottom->next = card;
                pile->bottom = card;
            }
            break;
        default:
            return POKER_ERR;
    }
    pile->card_num++;
    return POKER_OK;
}

static CARD_T *remove_card(PILE_T *pile, int type, int index)
{
    CARD_T *card = NULL;

    if (pile->top == NULL) return NULL;
    switch (type)
    {
        case POKER_FROM_TOP:
            card = pile->top;
            break;
        case POKER_FROM_BOTTOM:
            card = pile->bottom;
            break;
        case POKER_FROM_INDEX:
            card = pile->top;
            while (index-- > 0) card = card->next;
            break;
        default:
            return NULL;
    }
    if (card == pile->top)
    {
        pile->top = pile->top->next;
        if (pile->top != NULL) pile->top->prev = NULL;
    }
    else if (card == pile->bottom)
    {
        pile->bottom = pile->bottom->prev;
        if (pile->bottom != NULL) pile->bottom->next = NULL;
    }
    else
    {
        card->next->prev = card->prev;
        card->prev->next = card->next;
    }
    pile->card_num--;
    return card;
}

/* POKER_Create_Deck: create a deck of poker, priority is from spade A to club K
   * paremeter: int players -- how many players
                int joker_num -- how many joker cards
   * return value: the pointer to a deck of poker */
DECK_TP POKER_Create_Deck(int players, int joker_num)
{
    DECK_TP deck = NULL;
    int     color = 0;
    int     number = 0;
    int     joker = 0x01;
    CARD_T  *card = NULL;

    /* set random seed */
    srand(time(NULL));

    if ((joker_num < 0) || (players < 0)) return NULL;
    if ((deck = (DECK_T *)calloc(1, sizeof(DECK_T))) == NULL) return NULL;

    deck->joker_num = joker_num;
    deck->total_num = POKER_CARD_NUM + joker_num;
    deck->player_num = players;

    for (color = POKER_COLOR_SPADE; color >= POKER_COLOR_CLUB; color -= 0x10)
    {
        if ((card = (CARD_T *)calloc(1, sizeof(CARD_T))) == NULL)
        {
            POKER_Delete_Deck(&deck);
            return NULL;
        }
        card->card = color | POKER_NUM_A;
        insert_card(&deck->last_pile, card, POKER_FROM_BOTTOM);
        for (number = POKER_NUM_2; number <= POKER_NUM_K; number += 0x01)
        {
            if ((card = (CARD_T *)calloc(1, sizeof(CARD_T))) == NULL)
            {
                POKER_Delete_Deck(&deck);
                return NULL;
            }
            card->card = color | number;
            insert_card(&deck->last_pile, card, POKER_FROM_BOTTOM);
        }
    }
    while (joker_num-- > 0)
    {
        if ((card = (CARD_T *)calloc(1, sizeof(CARD_T))) == NULL)
        {
            POKER_Delete_Deck(&deck);
            return NULL;
        }
        card->card = POKER_COLOR_JOKER | joker;
        insert_card(&deck->last_pile, card, POKER_FROM_BOTTOM);
        joker += 0x01;
    }

    if ((deck->player = (PILE_T *)calloc(deck->player_num, sizeof(PILE_T))) == NULL)
    {
        POKER_Delete_Deck(&deck);
        return NULL;
    }
    return deck;
}

static void destroy_pile(PILE_T *pile)
{
    CARD_T *card = NULL;
    while (pile->top != NULL)
    {
        card = pile->top->next;
        free(pile->top);
        pile->top = card;
    }
}

/* POKER_Delete_Deck: delete a deck of poker */
void POKER_Delete_Deck(DECK_TP *deck)
{
    int idx = 0;
    if ((deck == NULL) || (*deck == NULL)) return;
    
    destroy_pile(&(*deck)->last_pile);
    destroy_pile(&(*deck)->trash_pile);
    if ((*deck)->player != NULL)
    {
        for (idx = 0; idx < (*deck)->player_num; idx++)
            destroy_pile(&(*deck)->player[idx]);
        free((*deck)->player);
    }
    free(*deck);
    *deck = NULL;
}

/* POKER_Get_TotalCardNum: get total card number,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: the total card number    */
int POKER_Get_TotalCardNum(DECK_TP deck)
{
    if (deck == NULL) return POKER_ERR;
    return deck->total_num;
}

/* POKER_Get_LastCardNum: get card number of last pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: the card number of last pile  */
int POKER_Get_LastCardNum(DECK_TP deck)
{
    if (deck == NULL) return POKER_ERR;
    return deck->last_pile.card_num;
}

/* POKER_Get_TrashCardNum: get card number of trash pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: the card number of trash pile  */
int POKER_Get_TrashCardNum(DECK_TP deck)
{
    if (deck == NULL) return POKER_ERR;
    return deck->trash_pile.card_num;
}

/* POKER_Get_PlayerNum: get card number of player pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player_no
   * return value: the card number of player pile  */
int POKER_Get_PlayerCardNum(DECK_TP deck, int player_no)
{
    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    return deck->player[player_no-1].card_num;
}

/* POKER_Show_LastCard: show a card from last_pile, 
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index
                int index -- from the index of last pile
   * return value: card number for success, POKER_ERR for failure   */
int POKER_Show_LastCard(DECK_TP deck, int type, int index)
{
    CARD_T *card = NULL;

    if (deck == NULL) return POKER_ERR;
    if (deck->last_pile.card_num == 0) return POKER_ERR;
    if (deck->last_pile.card_num <= index) return POKER_ERR;
    
    switch (type)
    {
        case POKER_FROM_TOP: return deck->last_pile.top->card;
        case POKER_FROM_BOTTOM: return deck->last_pile.bottom->card;
        case POKER_FROM_INDEX:
            card = deck->last_pile.top;
            while (index-- > 0) card = card->next;
            return card->card;
    }
    return POKER_ERR;
}

/* POKER_Show_TrashCard: show a card from trash_pile, 
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index
                int index -- from the index of last pile
   * return value: card number for success, POKER_ERR for failure   */
int POKER_Show_TrashCard(DECK_TP deck, int type, int index)
{
    CARD_T *card = NULL;

    if (deck == NULL) return POKER_ERR;
    if (deck->trash_pile.card_num == 0) return POKER_ERR;
    if (deck->trash_pile.card_num <= index) return POKER_ERR;
    
    switch (type)
    {
        case POKER_FROM_TOP: return deck->trash_pile.top->card;
        case POKER_FROM_BOTTOM: return deck->trash_pile.bottom->card;
        case POKER_FROM_INDEX:
            card = deck->trash_pile.top;
            while (index-- > 0) card = card->next;
            return card->card;
    }
    return POKER_ERR;
}

/* POKER_Show_PlayerCard: show a card from player_pile, 
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no
                int type -- from top, bottom or index
                int index -- from the index of last pile
   * return value: card number for success, POKER_ERR for failure   */
int POKER_Show_PlayerCard(DECK_TP deck, int player_no, int type, int index)
{
    CARD_T *card = NULL;

    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    if (deck->player[player_no-1].card_num == 0) return POKER_ERR;
    if (deck->player[player_no-1].card_num <= index) return POKER_ERR;
    
    switch (type)
    {
        case POKER_FROM_TOP: return deck->player[player_no-1].top->card;
        case POKER_FROM_BOTTOM: return deck->player[player_no-1].bottom->card;
        case POKER_FROM_INDEX:
            card = deck->player[player_no-1].top;
            while (index-- > 0) card = card->next;
            return card->card;
    }
    return POKER_ERR;
}

/* POKER_Deal_Card: deal a card from last_pile to a player,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index of last_pile
                int index -- from index of last_pile
                int player_no -- the player no
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Deal_Card(DECK_TP deck, int type, int index, int player_no)
{
    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    if (deck->last_pile.card_num == 0) return POKER_ERR;
    if (deck->last_pile.card_num <= index) return POKER_ERR;

    return insert_card(&deck->player[player_no-1], remove_card(&deck->last_pile, type, index), POKER_FROM_BOTTOM);
}

/* POKER_Transfer_PlayerCard: transfer a card from a player to another,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index of last_pile
                int index -- from index of last_pile
                int from_player_no -- the player no
                int to_player_no -- the player no
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Transfer_PlayerCard(DECK_TP deck, int type, int index, int from_player_no, int to_player_no)
{
    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < from_player_no) return POKER_ERR;
    if (deck->player_num < to_player_no) return POKER_ERR;
    if (deck->player[from_player_no-1].card_num == 0) return POKER_ERR;

    return insert_card(&deck->player[to_player_no-1], 
                       remove_card(&deck->player[from_player_no-1], type, index), 
                       POKER_FROM_BOTTOM);
}

/* POKER_Throw_LastCard: throw a card from last_pile to trash_pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index of last_pile
                int index -- from index of last_pile
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Throw_LastCard(DECK_TP deck, int type, int index)
{
    if (deck == NULL) return POKER_ERR;
    if (deck->last_pile.card_num == 0) return POKER_ERR;
    if (deck->last_pile.card_num <= index) return POKER_ERR;
    if (deck->trash_pile.card_num == deck->total_num) return POKER_ERR;
    
    return insert_card(&deck->trash_pile, remove_card(&deck->last_pile, type, index), POKER_FROM_BOTTOM);
}

/* POKER_Throw_PlayerCard: throw a card from player to trash_pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no
                int type -- from top, bottom or index of player's pile
                int index -- the index of player's pile
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Throw_PlayerCard(DECK_TP deck, int player_no, int type, int index)
{
    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    if (deck->player[player_no-1].card_num <= index) return POKER_ERR;
    if (deck->trash_pile.card_num == deck->total_num) return POKER_ERR;
    
    return insert_card(&deck->trash_pile, remove_card(&deck->player[player_no-1], type, index), POKER_FROM_BOTTOM);
}

static void move_card(PILE_T *pile, CARD_T *move, CARD_T *curr)
{
    /* no need to move if neighbor */
    if (curr->prev == move) return;

    /* check current is tail */
    if (pile->bottom == curr)
    {
        pile->bottom = curr->prev;
        pile->bottom->next = NULL;
    }
    else
    {
        curr->next->prev = curr->prev;
        curr->prev->next = curr->next;
    }

    /* check target is header */
    if (move == NULL)
    {
        pile->top->prev = curr;
        curr->next = pile->top;
        pile->top = curr;
        pile->top->prev = NULL;
    }
    else
    {
        curr->next = move->next;
        curr->prev = move;
        curr->prev->next = curr;
        curr->next->prev = curr;
    }
}


static void insertion_sort(PILE_T *pile, int (*comp_func)(int card1, int card2))
{
    CARD_T  *curr = NULL;
    CARD_T  *move = NULL;
    CARD_T  *tmp = NULL;
    int     rv = 0;

    curr = pile->top->next;
    while (curr != NULL)
    {
        tmp = curr->next;
        move = curr->prev;
        while (move != NULL)
        {
            rv = comp_func(move->card, curr->card);
            if (rv == 0) break;
            else if (rv < 0)
            {
                move_card(pile, move, curr);
                break;
            }
            if ((move = move->prev) == NULL) move_card(pile, move, curr);
        }
        curr = tmp;
    }
}

/* POKER_Dump_PlayerPile: dump a player's pile, the way appointed in dump_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
                void (*dump_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter */
void POKER_Dump_PlayerPile(DECK_TP deck, int player_no, void (*dump_func)(int index, int card, void *para), void *para)
{
    int     idx = 0;
    CARD_T  *tmp = NULL;

    if (deck == NULL) return;
    if (deck->player == NULL) return;
    if (deck->player_num < player_no) return;
    for (tmp = deck->player[player_no-1].top; tmp != NULL; tmp = tmp->next)
    {
        dump_func(idx++, tmp->card, para);
    }
}

/* POKER_Dump_LastPile: dump last pile, the way appointed in dump_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                void (*dump_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter */
void POKER_Dump_LastPile(DECK_TP deck, void (*dump_func)(int index, int card, void *para), void *para)
{
    int     idx = 0;
    CARD_T  *tmp = NULL;

    if (deck == NULL) return;
    for (tmp = deck->last_pile.top; tmp != NULL; tmp = tmp->next)
    {
        dump_func(idx++, tmp->card, para);
    }
}

/* POKER_Dump_TrashPile: dump trash pile, the way appointed in dump_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                void (*dump_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter */
void POKER_Dump_TrashPile(DECK_TP deck, void (*dump_func)(int index, int card, void *para), void *para)
{
    int     idx = 0;
    CARD_T  *tmp = NULL;

    if (deck == NULL) return;
    for (tmp = deck->trash_pile.top; tmp != NULL; tmp = tmp->next)
    {
        dump_func(idx++, tmp->card, para);
    }
}

/* POKER_Search_PlayerPile: search from player's pile, the way appointed in search_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
                int (*search_func)(int index, int card, void *para) -- the search rule function,
                void *para -- user parameter
   * return value: POKER_OK for found, POKER_NONE for none, POKER_ERR for failure
   * comment: if the search func return POKER_OK, means found and function will return, else keep loop searching    */
int POKER_Search_PlayerPile(DECK_TP deck, int player_no, 
                            int (*search_func)(int index, int card, void *para), void *para)
{
    int     idx = 0;
    CARD_T  *tmp = NULL;

    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    for (tmp = deck->player[player_no-1].top; tmp != NULL; tmp = tmp->next)
    {
        if (search_func(idx++, tmp->card, para) == POKER_OK) return POKER_OK;
    }
    return POKER_NONE;
}

/* POKER_Search_LastPile: search from last pile, the way appointed in search_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*search_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter
   * return value: POKER_OK for found, POKER_NONE for none, POKER_ERR for failure
   * comment: if the search func return POKER_OK, means found and function will return, else keep loop searching    */
int POKER_Search_LastPile(DECK_TP deck, int (*search_func)(int index, int card, void *para), void *para)
{
    int     idx = 0;
    CARD_T  *tmp = NULL;

    if (deck == NULL) return POKER_ERR;
    for (tmp = deck->last_pile.top; tmp != NULL; tmp = tmp->next)
    {
        if (search_func(idx++, tmp->card, para) == POKER_OK) return POKER_OK;
    }
    return POKER_NONE;
}

/* POKER_Search_TrashPile: search trash pile, the way appointed in search_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*search_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter
   * return value: POKER_OK for found, POKER_NONE for none, POKER_ERR for failure
   * comment: if the search func return POKER_OK, means found and function will return, else keep loop searching    */
int POKER_Search_TrashPile(DECK_TP deck, int (*search_func)(int index, int card, void *para), void *para)
{
    int     idx = 0;
    CARD_T  *tmp = NULL;

    if (deck == NULL) return POKER_ERR;
    for (tmp = deck->trash_pile.top; tmp != NULL; tmp = tmp->next)
    {
        if (search_func(idx++, tmp->card, para) == POKER_OK) return POKER_OK;
    }
    return POKER_NONE;
}

/* POKER_Sort_LastPile: sort last pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*comp_func)(int card1, int card2) -- the compare rule function
                    rv definition: = 0 means card1 is equal card2
                                   > 0 means card1 should be in back of card2
                                   < 0 means card1 should be in front of card2
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Sort_LastPile(DECK_TP deck, int (*comp_func)(int card1, int card2))
{
    if (deck == NULL) return POKER_ERR;
    if (deck->last_pile.card_num == 0) return POKER_OK;
    
    insertion_sort(&deck->last_pile, comp_func);
    return POKER_OK;
}

/* POKER_Sort_TrashPile: sort trash pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*comp_func)(int card1, int card2) -- the compare rule function
                    rv definition: = 0 means card1 is equal card2
                                   > 0 means card1 should be in back of card2
                                   < 0 means card1 should be in front of card2
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Sort_TrashPile(DECK_TP deck, int (*comp_func)(int card1, int card2))
{
    if (deck == NULL) return POKER_ERR;
    if (deck->trash_pile.card_num == 0) return POKER_OK;
    
    insertion_sort(&deck->trash_pile, comp_func);
    return POKER_OK;
}

/* POKER_Sort_PlayerPile: sort a player's pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
                int (*comp_func)(int card1, int card2) -- the compare rule function
                    rv definition: = 0 means card1 is equal card2
                                   > 0 means card1 should be in back of card2
                                   < 0 means card1 should be in front of card2
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Sort_PlayerPile(DECK_TP deck, int player_no, int (*comp_func)(int card1, int card2))
{
    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    if (deck->player[player_no-1].card_num == 0) return POKER_OK;
    
    insertion_sort(&deck->player[player_no-1], comp_func);
    return POKER_OK;
}

static void shuffle_pile(PILE_T *pile, int total)
{
    int     rand_num = 0;
    CARD_T  *card = NULL;
    CARD_T  *card_swap = NULL;
    
    for (card = pile->top; card != NULL; card = card->next)
    {
        rand_num = rand() % total;
        card_swap = pile->top;
        while (rand_num-- > 0) card_swap = card_swap->next;
        swap(&card->card, &card_swap->card);
    }
}

/* POKER_Shuffle_LastPile: shuffle last pile
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Shuffle_LastPile(DECK_TP deck)
{
    if (deck == NULL) return POKER_ERR;
    shuffle_pile(&deck->last_pile, deck->last_pile.card_num);
    return POKER_OK;    
}

/* POKER_Shuffle_TrashPile: shuffle trash pile and combine behind last pile
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Shuffle_TrashPile(DECK_TP deck)
{
    if (deck == NULL) return POKER_ERR;
    shuffle_pile(&deck->trash_pile, deck->trash_pile.card_num);
    while (deck->trash_pile.card_num != 0)
    {
        insert_card(&deck->last_pile, remove_card(&deck->trash_pile, POKER_FROM_TOP, 0), POKER_FROM_BOTTOM);
    }
    return POKER_OK;
}

/* POKER_Shuffle_PlayerPile: shuffle a player's pile
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Shuffle_PlayerPile(DECK_TP deck, int player_no)
{
    if (deck == NULL) return POKER_ERR;
    if (deck->player == NULL) return POKER_ERR;
    if (deck->player_num < player_no) return POKER_ERR;
    shuffle_pile(&deck->player[player_no-1], deck->player[player_no-1].card_num);
    return POKER_OK;
}

