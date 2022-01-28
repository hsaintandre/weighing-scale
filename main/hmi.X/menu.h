#include <xc.h>
#include "definitions.h"


char msjBLANK[16] = "               ";

/**
 * @Summary
 *      Loads all parameters if saved on memory or defaults if not
 */
void menu_init ();

/**
 * @Summary
 *      Print on screen menu level 1 with specified colored line
 * @param colorRow
 */
void menu_1 (char colorRow);

/**
 * @Summary
 *      Print on screen menu level 2 with specified colored line
 * @param colorRow
 */
void menu_2 (char colorRow);

void menu_21 (char colorRow);
void menu_22 (char colorRow);
void menu_23 (char colorRow);
void menu_24 (char colorRow);
void menu_25 (char colorRow);
void menu_26 (char colorRow);
void menu_27 (char colorRow);

/**
 * @Summary
 *      Print on screen menu level 2 with specified colored line
 * @param colorRow
 */
void menu_3 (char colorRow);

void menu_31 (char colorRow);
void menu_32 (char colorRow);

/**
 * @Summary
 *      Print on screen menu level 2 with specified colored line
 * @param colorRow
 */
void menu_4 (char colorRow);

void menu_41 (char colorRow);
void menu_42 (char colorRow);
void menu_43 (char colorRow);
void menu_44 (char colorRow);
void menu_45 (char colorRow);
void menu_46 (char colorRow);
void menu_47 (char colorRow);

/**
 * @Summary
 *      Print on screen menu level 2 with specified colored line
 * @param colorRow
 */
void menu_5 (char colorRow);

void menu_51 (char colorRow);
void menu_52 (char colorRow);
void menu_53 (char colorRow);

/**
 * @Summary
 *      Print on screen menu level 2 with specified colored line
 * @param colorRow
 */
void menu_6 (char colorRow);

void menu_61 (char colorRow);
void menu_62 (char colorRow);

/**
 * @Summary
 *      This function will navigate through
 * @param opt
 * @param rowColor
 */
void menu_navigate (char opt, char rowColor);

/**
 * @Summary
 *      Updates the menu page and the selected row. Returns whether the selection
 *      was a folder or an option. Also an optional parameter of text to print can
 *      be used
 * @param menuShown
 * @param menuRow
 * @param command
 * @param text
 * @return
 */
char menu_next (char *menuShown, char *menuRow, char command, char *menuText);