/* poker library header file */
#ifndef _POKER_H_
#define _POKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define POKER_OK       0
#define POKER_NONE     -1
#define POKER_ERR      -99

#define POKER_MASK_COLOR  0xF0
#define POKER_MASK_NUM    0x0F

#define POKER_COLOR_SPADE    0x50
#define POKER_COLOR_HEART    0x40
#define POKER_COLOR_DIAMOND  0x30
#define POKER_COLOR_CLUB     0x20
#define POKER_COLOR_JOKER    0x10

#define POKER_NUM_A    0x0E
#define POKER_NUM_K    0x0D
#define POKER_NUM_Q    0x0C
#define POKER_NUM_J    0x0B
#define POKER_NUM_10   0x0A
#define POKER_NUM_9    0x09
#define POKER_NUM_8    0x08
#define POKER_NUM_7    0x07
#define POKER_NUM_6    0x06
#define POKER_NUM_5    0x05
#define POKER_NUM_4    0x04
#define POKER_NUM_3    0x03
#define POKER_NUM_2    0x02

#define POKER_CARD_NUM    52

#define POKER_FROM_INDEX  0
#define POKER_FROM_TOP    1
#define POKER_FROM_BOTTOM 2

typedef struct card_s CARD_T;
typedef struct pile_s PILE_T;
typedef struct deck_s DECK_T;
typedef DECK_T* DECK_TP;

/* POKER_Color: check the color of card
    * parameter: int card -- the card
    * comment: the color will be POKER_COLOR_SPADE ~ POKER_COLOR_JOKER  */
#define POKER_Color(card) (POKER_MASK_COLOR&card)

/* POKER_Num: check the number of card
    * parameter: int card -- the card
    * comment: the number will be POKER_NUM_A ~ POKER_NUM_K */
#define POKER_Num(card)   (POKER_MASK_NUM&card)

/* POKER_Create_Deck: create a deck of poker, priority is from spade A to club K
   * paremeter: int players -- how many players
                int joker_num -- how many joker cards
   * return value: the pointer to a deck of poker */
DECK_TP POKER_Create_Deck(int players, int joker_num);

/* POKER_Delete_Deck: delete a deck of poker */
void POKER_Delete_Deck(DECK_TP *deck);

/* POKER_Get_TotalCardNum: get total card number,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: the total card number    */
int POKER_Get_TotalCardNum(DECK_TP deck);

/* POKER_Get_LastCardNum: get card number of last pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: the card number of last pile  */
int POKER_Get_LastCardNum(DECK_TP deck);

/* POKER_Get_TrashCardNum: get card number of trash pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: the card number of trash pile  */
int POKER_Get_TrashCardNum(DECK_TP deck);

/* POKER_Get_PlayerNum: get card number of player pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player_no
   * return value: the card number of player pile  */
int POKER_Get_PlayerCardNum(DECK_TP deck, int player_no);

/* POKER_Show_LastCard: show a card from last_pile, 
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index
                int index -- from the index of last pile
   * return value: card number for success, POKER_ERR for failure   */
int POKER_Show_LastCard(DECK_TP deck, int type, int index);

/* POKER_Show_TrashCard: show a card from trash_pile, 
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index
                int index -- from the index of last pile
   * return value: card number for success, POKER_ERR for failure   */
int POKER_Show_TrashCard(DECK_TP deck, int type, int index);

/* POKER_Show_PlayerCard: show a card from player_pile, 
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no
                int type -- from top, bottom or index
                int index -- from the index of last pile
   * return value: card number for success, POKER_ERR for failure   */
int POKER_Show_PlayerCard(DECK_TP deck, int player_no, int type, int index);

/* POKER_Deal_Card: deal a card from last_pile to a player,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index of last_pile
                int index -- from index of last_pile
                int player_no -- the player no
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Deal_Card(DECK_TP deck, int type, int index, int player_no);

/* POKER_Transfer_PlayerCard: transfer a card from a player to another,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index of last_pile
                int index -- from index of last_pile
                int from_player_no -- the player no
                int to_player_no -- the player no
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Transfer_PlayerCard(DECK_TP deck, int type, int index, int from_player_no, int to_player_no);

/* POKER_Throw_LastCard: throw a card from last_pile to trash_pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int type -- from top, bottom or index of last_pile
                int index -- from index of last_pile
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Throw_LastCard(DECK_TP deck, int type, int index);

/* POKER_Throw_PlayerCard: throw a card from player to trash_pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no
                int type -- from top, bottom or index of player's pile
                int index -- the index of player's pile
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Throw_PlayerCard(DECK_TP deck, int player_no, int type, int index);

/* POKER_Dump_PlayerPile: dump a player's pile, the way appointed in dump_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
                void (*dump_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter */
void POKER_Dump_PlayerPile(DECK_TP deck, int player_no, void (*dump_func)(int index, int card, void *para), void *para);

/* POKER_Dump_LastPile: dump last pile, the way appointed in dump_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                void (*dump_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter */
void POKER_Dump_LastPile(DECK_TP deck, void (*dump_func)(int index, int card, void *para), void *para);

/* POKER_Dump_TrashPile: dump trash pile, the way appointed in dump_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                void (*dump_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter */
void POKER_Dump_TrashPile(DECK_TP deck, void (*dump_func)(int index, int card, void *para), void *para);

/* POKER_Sort_LastPile: sort last pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*comp_func)(int card1, int card2) -- the compare rule function
                    rv definition: = 0 means card1 is equal card2
                                   > 0 means card1 should be in back of card2
                                   < 0 means card1 should be in front of card2
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Sort_LastPile(DECK_TP deck, int (*comp_func)(int card1, int card2));

/* POKER_Sort_TrashPile: sort trash pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*comp_func)(int card1, int card2) -- the compare rule function
                    rv definition: = 0 means card1 is equal card2
                                   > 0 means card1 should be in back of card2
                                   < 0 means card1 should be in front of card2
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Sort_TrashPile(DECK_TP deck, int (*comp_func)(int card1, int card2));

/* POKER_Sort_PlayerPile: sort a player's pile,
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
                int (*comp_func)(int card1, int card2) -- the compare rule function
                    rv definition: = 0 means card1 is equal card2
                                   > 0 means card1 should be in back of card2
                                   < 0 means card1 should be in front of card2
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Sort_PlayerPile(DECK_TP deck, int player_no, int (*comp_func)(int card1, int card2));

/* POKER_Shuffle_LastPile: shuffle last pile
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Shuffle_LastPile(DECK_TP deck);

/* POKER_Shuffle_TrashPile: shuffle trash pile and combine behind last pile
   * parameter: DECK_TP deck -- the pointer to a deck of poker
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Shuffle_TrashPile(DECK_TP deck);

/* POKER_Shuffle_PlayerPile: shuffle a player's pile
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
   * return value: POKER_OK for success, POKER_ERR for fail */
int POKER_Shuffle_PlayerPile(DECK_TP deck, int player_no);

/* POKER_Search_PlayerPile: search from player's pile, the way appointed in search_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int player_no -- the player no.
                int (*search_func)(int index, int card, void *para) -- the search rule function,
                void *para -- user parameter
   * return value: POKER_OK for found, POKER_NONE for none, POKER_ERR for failure
   * comment: if the search func return POKER_OK, means found and function will return, else keep loop searching    */
int POKER_Search_PlayerPile(DECK_TP deck, int player_no, 
                            int (*search_func)(int index, int card, void *para), void *para);

/* POKER_Search_LastPile: search from last pile, the way appointed in search_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*search_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter
   * return value: POKER_OK for found, POKER_NONE for none, POKER_ERR for failure
   * comment: if the search func return POKER_OK, means found and function will return, else keep loop searching    */
int POKER_Search_LastPile(DECK_TP deck, int (*search_func)(int index, int card, void *para), void *para);

/* POKER_Search_TrashPile: search trash pile, the way appointed in search_func
   * parameter: DECK_TP deck -- the pointer to a deck of poker
                int (*search_func)(int index, int card, void *para) -- the dump rule function,
                void *para -- user parameter
   * return value: POKER_OK for found, POKER_NONE for none, POKER_ERR for failure
   * comment: if the search func return POKER_OK, means found and function will return, else keep loop searching    */
int POKER_Search_TrashPile(DECK_TP deck, int (*search_func)(int index, int card, void *para), void *para);

#endif

