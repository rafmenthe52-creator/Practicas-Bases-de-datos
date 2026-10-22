#include <stdio.h>
#include <stdlib.h>

/* #include "menu_template.h" */
/* #ifndef A2BIS_MENU_TEMPLATE_H */
/* #define A2BIS_MENU_TEMPLATE_H */

/* Prototypes:
 * Since in this example this functions are not used outside menu_template
 * it does not make sense to create an include file and export them
 * BUT in your code it will make sense to break this file in many files
 * and create several include files with the prototypes
 *
 * A static function in C is a function that has a scope that is limited
 * to its object file. This means that the static function is only
 * visible in its object file. Therefore,
 * you need to remove the static keyword from the declaration
 * if the function is declared in a file and used in another
 * */

static int ShowMainMenu();
static void ShowRhymeMenu();
static int ShowRhymeSubMenu();
static void ShowFairyMenu();
static int ShowFairySubMenu();
static void PrintZub();
static void PrintBigFatGit();
static void PrintAliens();
static void PrintMary();
static void PrintJack();
static void PrintLittleBoPeep();

/* #endif //A2BIS_MENU_TEMPLATE_H */

/**
 * @file menu_template.c
 * @author rmarabini
 * @date 15 April 2020
 * @brief File containing an example of a menu system with submenus
 *
 * The following program allows a user to select a nursery rhyme
 * or a fairy tail from a menu. The menu has two levels, the top
 * one allows to select between fairy tails or nursery rhymes. The
 * second level (sub-menus) allow to select either a particular
 * rhyme or tail.
 *
 * @see https://bytes.com/topic/c/answers/567136-creating-menus-sub-menus
 */


/**
 * @brief Entry point, shows and process main menu.
 *
 * This function implements a loop that prints the main
 * menu, analyze the user selection and calls the
 * corresponding submenu
 *
 * @return 0 if no error
 * @author rmarabini
 */
int main(void) {
    int nChoice = 0;

    do {
        nChoice = ShowMainMenu();
        switch (nChoice) {
            case 1: {
                ShowRhymeMenu();
            }
                break;

            case 2: {
                ShowFairyMenu();
            }
                break;

            case 3: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 3);
    return 0;
}


/**
 * @brief prints main menu and allows to select an option.
 *
 *
 * @return selected entry in the menu
 * @author rmarabini
 */

int ShowMainMenu() {
    int nSelected = 0;
    char buf[16]; /* buffer for reading user input
                     in theory we only need 1 char
                     but a large buffer will handle
                     more gracefully wrong inputs
                     as 'qwerty' */

    do {
        printf("This is a nursery rhyme and fairy stories program\n");
        printf("You can choose to display a number of different nursery rhymes and fairy stories\n\n");

        printf(" (1) Nursery rhymes\n"
               " (2) Fairy stories\n"
               " (3) Quit\n\n"
               "Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 3)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;
}

/**
 * @brief Shows and processes the rhyme menu.
 *
 * This function implements a loop that prints the rhyme
 * menu, analyze the user selection and prints
 * the selected rhyme
 * @return void
 * @author rmarabini
 */

void ShowRhymeMenu() {
    int nChoice = 0;
    do {
        nChoice = ShowRhymeSubMenu();
        switch (nChoice) {

            case 1: {
                PrintMary();
            }
                break;

            case 2: {
                PrintJack();
            }
                break;

            case 3: {
                PrintLittleBoPeep();
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

/**
 * @brief prints rhyme menu and allows to select an option.
 *
 * @return selected option
 * @author rmarabini
 */

 int ShowRhymeSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Mary had a little lamb\n"
               " (2) Jack and Jill\n"
               " (3) Little Bo Peep\n"
               " (4) Quit\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");


        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

/**
 * @brief Shows and processes the fairy menu.
 *
 * This function implements a loop that prints the fairy
 * menu, analyze the user selection and prints
 * the selected fairy tail
 *
 * @return selected option
 * @author rmarabini
 */

void ShowFairyMenu() {
    int nChoice = 0;

    do {
        nChoice = ShowFairySubMenu();
        switch (nChoice) {
            case 1: {
                PrintZub();
            }
                break;

            case 2: {
                PrintBigFatGit();
            }
                break;

            case 3: {
                PrintAliens();
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

/**
 * @brief prints the fairy menu and allows to select an option.
 *
 * @return selected option
 * @author rmarabini
 */
 int ShowFairySubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) The zub zubs\n"
               " (2) The BFG\n"
               " (3) PrintAliens\n"
               " (4) Quit\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

/**
 * @brief prints Rhyme Mary
 *
 * @return void
 * @author rmarabini
 *
 */
 void PrintMary() {
    printf("Mary had a little lamb\n fleese white as snow\n\n\n");
}

/**
 * @brief prints Rhyme Jack
 *
 * @return void
 * @author rmarabini
 *
 */
void PrintJack() {
    printf("Jack and jill went up the hill to fetch water\nThen they fell down\n\n\n");
}

/**
 * @brief prints Rhyme LittleBoPee
 *
 * @return void
 * @author rmarabini
 *
 */
void PrintLittleBoPeep() {
    printf("Little bo peep had sheep\nthen they came home\n\n\n");
}

/**
 * @brief prints fairy tail LittleBoPee
 *
 * @return void
 * @author rmarabini
 *
 */
 void PrintZub() {
    printf("zub zub zub zub zub zub zub zub\n\n\n");
}

/**
 * @brief prints fairy tail LittleBoPee
 *
 * @return void
 * @author rmarabini
 *
 */
void PrintBigFatGit() {
    printf("im big and fat and a git\n\n\n");
}

/**
 * @brief prints fairy tail LittleBoPee
 *
 * @return void
 * @author rmarabini
 *
 */

void PrintAliens() {
    printf("take us to your leader\n\n\n");
}
     
     


