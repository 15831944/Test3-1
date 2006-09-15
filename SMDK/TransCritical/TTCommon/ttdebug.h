bool bTTDebug;

#ifndef TTSOCK

void p(char *s, double  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  Log.Message(MMsg_Note, msgstr);
}
void p(char *s, bool  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  Log.Message(MMsg_Note, msgstr);
}
void p(char *s, int  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  Log.Message(MMsg_Note, msgstr);
}
void p(char*s) 
{
  if (!bTTDebug) return;
  Log.Message(MMsg_Note, s);
}
#endif

#ifdef TTSOCK

void p(char *s, double  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  Log.Message(MMsg_Note, msgstr);
}
void p(char *s, bool  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  Log.Message(MMsg_Note, msgstr);
}
void p(char *s, int  x) {
  if (!bTTDebug) return;
  char msgstr[80];
  sprintf(msgstr, s, x);
  Log.Message(MMsg_Note, msgstr);
}
void p(char*s) 
{
  if (!bTTDebug) return;
  Log.Message(MMsg_Note, s);
}
#endif

  
