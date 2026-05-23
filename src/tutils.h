// utils for tui
#ifndef TUI_UTILS_H
#define TUI_UTILS_H

#define CSI "\x1b["

/* foreground (30-37) */
#define K   CSI "30m"
#define R   CSI "31m"
#define G   CSI "32m"
#define Y   CSI "33m"
#define B   CSI "34m"
#define M   CSI "35m"
#define C   CSI "36m"
#define W   CSI "37m"

/* bold foreground (1;3x) */
#define KK  CSI "1;30m"
#define RR  CSI "1;31m"
#define GG  CSI "1;32m"
#define YY  CSI "1;33m"
#define BB  CSI "1;34m"
#define MM  CSI "1;35m"
#define CC  CSI "1;36m"
#define WW  CSI "1;37m"

/* background (40-47) */
#define LK  CSI "40m"
#define LR  CSI "41m"
#define LG  CSI "42m"
#define LY  CSI "43m"
#define LB  CSI "44m"
#define LM  CSI "45m"
#define LC  CSI "46m"
#define LW  CSI "47m"

/* bright background (100-107) — "LXX" form */
#define LKK CSI "100m"
#define LRR CSI "101m"
#define LGG CSI "102m"
#define LYY CSI "103m"
#define LBB CSI "104m"
#define LMM CSI "105m"
#define LCC CSI "106m"
#define LWW CSI "107m"

/* attributes */
#define X     CSI "0m"   /* reset */
#define BOLD  CSI "1m"
#define DIM   CSI "2m"
#define ITAL  CSI "3m"
#define UNDER CSI "4m"
#define REV   CSI "7m"

/* 256-color and truecolor escape hatches (these need printf, not concat) */
#define FG256(n)      CSI "38;5;" #n "m"
#define BG256(n)      CSI "48;5;" #n "m"
#define FG_RGB(r,g,b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define BG_RGB(r,g,b) "\x1b[48;2;" #r ";" #g ";" #b "m"

/* lines */
#define BOX_UD     "\u2502"   /* │ */
#define BOX_RL     "\u2500"   /* ─ */
 
/* corners */
#define BOX_DR     "\u250C"   /* ┌ */
#define BOX_DL     "\u2510"   /* ┐ */
#define BOX_UR     "\u2514"   /* └ */
#define BOX_UL     "\u2518"   /* ┘ */
 
/* tees */
#define BOX_UDR    "\u251C"   /* ├ */
#define BOX_UDL    "\u2524"   /* ┤ */
#define BOX_DRL    "\u252C"   /* ┬ */
#define BOX_URL    "\u2534"   /* ┴ */
 
/* cross */
#define BOX_UDRL   "\u253C"   /* ┼ */

/* screen */
#define CLEAR    CSI "2J"
#define HOME     CSI "H"

#endif
