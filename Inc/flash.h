#include "stm32f4xx.h"

void SaveCalValues(unsigned int *buf, unsigned int nWords);
void LoadCalValues(unsigned int *buf, unsigned int nWords);
void WriteNoteCounter (void);
unsigned int ReadNoteCounter (void);
void ResetNoteCounter(void);
void EraseFlash(void);
void Go_To_User_App(void);
void ReadCurrentVersion (void);

extern char ver_in_flash[8];
