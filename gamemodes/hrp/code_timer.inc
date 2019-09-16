#define Code_ExTimer_Begin(%0) new CET_%0 = GetTickCount()
#define Code_ExTimer_End(%0) (GetTickCount() - CET_%0)