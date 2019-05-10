//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2013-2019 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     tzstrip.c
///
///  \brief    Tool to strip timezone information.
///
///  \author   HFS: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

typedef struct {
   // Rule  NAME  FROM  TO  TYPE  IN  ON  AT  SAVE  LETTER/S
    time_t from;
    time_t to;
    char * type;
    char * in;
    char * on;
    char * at;
    char *  save;
    char * letters;
}tRuleRule;

typedef struct {
   // Rule  NAME  FROM  TO  TYPE  IN  ON  AT  SAVE  LETTER/S
    char * name;
    //tRuleRule * rule;
    GSList * list;
}tRule;



typedef struct {
    //Zone  NAME                GMTOFF  RULES/SAVE  FORMAT  [UNTILYEAR [MONTH [DAY [TIME]]]]
    //char * name;
    char *  gmtoff;
    tRule *  rule;
    char * save;
    char * format;
    time_t until;
    char * sUntil;
}tSubZone;

typedef struct {
  char * name;
  FILE * fp;
  int counter;
  int canPrint;
  tSubZone * lastSub;
}tPrintRule;
typedef struct {
    char * name;
    GSList * list;
    //tSubZone * zone;
}tZone;

typedef void(*tIterDirFunc)(const char * path, void * arg);

GHashTable * ruleHash = NULL;
GHashTable * zoneHash = NULL;
GSList * linkList = NULL;
FILE * strippedout = NULL;

#if 0
void dbgPrintRule( const tRuleRule * rule,const char * name)
{
  char buf[64];
  printf("Rule\t");
  printf("%s\t",name);
  strftime(buf,sizeof(buf),"%Y",localtime(&rule->from));
  printf("%s\t",buf);
  if(rule->to == INT_MAX)
  {
   printf("max\t");
  }
  else if(rule->from == rule->to)
  {
    printf("only\t");
  }
  else
  {
    strftime(buf,sizeof(buf),"%Y",localtime(&rule->to));
    printf("%s\t",buf);
  }
  printf("%s\t",rule->type);
  printf("%s\t",rule->in);
  printf("%s\t",rule->on);
  printf("%s\t",rule->at);
  printf("%s\t",rule->save);
  printf("%s\n",rule->letters);
}
#endif
tRuleRule * newRule(time_t from,
                    time_t to,
                    char * type,
                    char * in,
                    char * on,
                    char *  at,
                    char * save,
                    char * letters)
{
  tRuleRule * new =NULL;

  new = malloc(sizeof(tRuleRule));
  if(new == NULL) return NULL;
  new->from = from;
  new->to = to;
  new->type = NULL;
  new->in = NULL;
  new->on = NULL;
  new->at = NULL;
  new->save = NULL;
  new->letters = NULL;
  if(type != NULL)
  {
    new->type = strdup(type);
  }
  if(in != NULL)
  {
    new->in = strdup(in);
  }
  if(on != NULL)
  {
    new->on = strdup(on);
  }
  if(at != NULL)
  {
    new->at = strdup(at);
  }
  if(save != NULL)
  {
    new->save = strdup(save);
  }
  if(letters != NULL)
  {
    new->letters = strdup(letters);
  }
  return new;
}


gint CmpRules(gconstpointer a, gconstpointer b)
{
  tRuleRule * rA = (tRuleRule*)a;
  tRuleRule * rB = (tRuleRule*)b;
  gint ret=1;

  do {
    //if(rA->at != rB->at)break;
    if(strcmp(rA->at,rB->at))break;
    if(rA->from != rB->from)break;
    //if(rA->in != rB->in)break;
    if(strcmp(rA->in,rB->in))break;
    //if(rA->letters != rB->letters)break;
    if(strcmp(rA->letters,rB->letters))break;
    //if(rA->on != rB->on)break;
    if(strcmp(rA->on,rB->on))break;
    //if(rA->save != rB->save)break;
    if(strcmp(rA->save,rB->save))break;
    if(rA->to != rB->to)break;
    //if(rA->type != rB->type)break;
    if(strcmp(rA->type,rB->type))break;
    ret = 0;
  }while(0);

  return ret;
}

void freeRule(tRuleRule * delRule)
{
  free(delRule->in);
  free(delRule->letters);
  free(delRule->on);
  free(delRule->type);
  free(delRule);
}

tRule * GetRule(char * name)
{
  return (tRule*)g_hash_table_lookup(ruleHash,name);
}

tRule * GetOrSetRule(char * name,tRuleRule * newRule)
{
  tRule * fromHash = NULL;
  tRule * new = NULL;
  fromHash = GetRule(name);
  if(fromHash != NULL)
  {
    GSList * listEl = g_slist_find_custom(fromHash->list,newRule,CmpRules);
    if(listEl == NULL)
    {
      fromHash->list = g_slist_append(fromHash->list, newRule);
    }
    else
    {
      freeRule(newRule);
    }
    return fromHash;
  }
  else
  {
    new = malloc(sizeof(tRule));
    new->list = NULL;
    new->name = NULL;
    new->name = strdup(name);
    new->list = g_slist_append(new->list, newRule);
    g_hash_table_insert(ruleHash,new->name,new);
    return new;
  }
}

void IterDir(const char * dirPath,tIterDirFunc callback, void * arg)
{
  DIR * dirPointer;
  struct dirent *dir;
  struct stat s = {0};

  dirPointer = opendir(dirPath);
  if (dirPointer == NULL)
  {
      perror("open source dir");
      exit(EXIT_FAILURE);
  }
  while ((dir = readdir(dirPointer)) != NULL)
  {
    char path[4096];
    sprintf(path,"%s/%s",dirPath,dir->d_name);
    if (stat(path, &s))
    {
      fprintf(stderr, "read file ");
      perror(path);
      exit(EXIT_FAILURE);
    }

    if(!(s.st_mode & (S_IFLNK | S_IFREG)))
    {
      printf("%s is not a regular file!\n",dir->d_name);
      continue;
    }
    //if(s.st_mode & (S_IFLNK))
    //if(S_ISLNK(s.st_mode))
    if((s.st_mode & S_IFMT)==S_IFLNK)
    {
      char buf[4096];
      ssize_t szbuf;
      //printf("%s is link\n",path);
      szbuf=readlink(path,buf,sizeof(buf));
      buf[szbuf]=0;
      //printf("szbuf = %d -> %s\n",szbuf,buf);
    }
    if(!(s.st_mode & (S_IRUSR | S_IWUSR)))
    {
      printf("access to %s not permitted\n",path);
      continue;
    }

    //printf("%s\n", dir->d_name);
    if(callback != NULL)
    {
      callback(path, arg);
    }
  }

  closedir(dirPointer);
}

int IsRule(char * line,size_t szLine)
{
  const char indicator1[] = "Rule";
  if(szLine < strlen(indicator1))return 0;
  if(strncmp(line,indicator1,strlen(indicator1)))return 0;

  return 1;
}

#define ZEROTIME(x) do{\
                     memset(&x,0,sizeof(struct tm));\
                     x.tm_mday = 1;\
                     }while(0);

char * parseRule(tRuleRule ** rule,char *line,size_t szLine)
{
  const char * delim = "\t";
  char buf[64];
  char * name;
  time_t from;
  time_t to;
  char * type;
  char * in;
  char * on;
  char * at;
  char * save;
  char * letters;
  char * ptr=line;
  char * parseStr = line;
  char * save_ptr;
  struct tm tmTmp;
  size_t i;

  for(i=0; i< szLine;i++)
  {
    if(line[i] == ' ')
    {
      line[i] = '\t';
    }
    if(line[i] == '\n')
    {
      line[i] = '\0';
    }
  }

  //Rule
  ptr=strtok_r(parseStr, delim, &save_ptr);
  //printf("    \t");
  //name
  ptr=strtok_r(NULL, delim, &save_ptr);
  name = strdup(ptr);
  //printf("%s\t",name);

  //from
  ptr=strtok_r(NULL, delim, &save_ptr);
  //printf("%s\t",ptr);
  ZEROTIME(tmTmp);
  tmTmp.tm_year = atoi(ptr)-1900;
  //tmTmp.tm_mday = 2;
  from = mktime(&tmTmp);
  //from = mktime(&tmTmp);
 // strftime(buf,sizeof(buf),"%Y",localtime(&from));
  strftime(buf,sizeof(buf),"%Y",localtime(&from));
  //printf("%s\t",buf);
  //to
  ptr=strtok_r(NULL, delim, &save_ptr);
  //printf("%s\t",ptr);
  if(!strcmp(ptr,"only"))
  {
    //tmTmp.tm_year++;
    to = mktime(&tmTmp);
  }
  else if(!strcmp(ptr, "max"))
  {
    to = INT_MAX;
  }
  else if(isdigit(*ptr))
  {
    ZEROTIME(tmTmp);
    tmTmp.tm_year = atoi(ptr)-1900;
    to = mktime(&tmTmp);
  }
  else
  {
    printf("error unexpeced value for TO: %s\n", ptr);
    exit(EXIT_FAILURE);
  }
  strftime(buf,sizeof(buf),"%Y",localtime(&to));
  //printf("%s\t",buf);

  //type
  ptr=strtok_r(NULL, delim, &save_ptr);
  type = strdup(ptr);
  //printf("%s\t",type);

  //in
  ptr=strtok_r(NULL, delim, &save_ptr);
  in = strdup(ptr);
  //printf("%s\t",in);

  //on
  ptr=strtok_r(NULL, delim, &save_ptr);
  on = strdup(ptr);
  //printf("%s\t",on);

  //at
  ptr=strtok_r(NULL, delim, &save_ptr);
  at = strdup(ptr);
  //printf("%s\t",at);

  //save
  ptr=strtok_r(NULL, delim, &save_ptr);
  save = strdup(ptr);
  //printf("%s\t",save);

  //letters
  ptr=strtok_r(NULL, delim, &save_ptr);
  letters = strdup(ptr);
  //printf("%s\t",letters);

  //printf("\n");

  *rule = newRule(from,to,type,in,on,at,save,letters);

  return name;
}

gint DepricatedRule(gconstpointer list, gconstpointer null)
{
  time_t t = time(NULL);
  struct tm now;
  struct tm to;
  tRuleRule * rList= (tRuleRule*)list;
  (void)null;

  localtime_r(&t,&now);
  localtime_r(&rList->to,&to);
  if(to.tm_year+1 < now.tm_year)
  {
   return 0;
  }
  else
  {
   return 1;
  }
}

void RemoveDeprecatedRules(tRule *fromHash)
{
  GSList * listEl = g_slist_find_custom(fromHash->list,NULL,DepricatedRule);

  while(listEl != NULL)
  {
    tRuleRule * rule = (tRuleRule*)listEl->data;
    //listEl->data = NULL;
    fromHash->list = g_slist_remove(fromHash->list,(gconstpointer) listEl->data);
    freeRule(rule);
    listEl = g_slist_find_custom(fromHash->list,NULL,DepricatedRule);
  }
}

gint MostRecentRule(gconstpointer list, gconstpointer check)
{
  tRuleRule * rList = (tRuleRule*)list;
  tRuleRule * rCheck = (tRuleRule*)check;
  gint ret=1;

  do {

    if(strcmp(rList->letters,"-"))break;
    if(strcmp(rList->save,"0"))break;

    if(rList->to > rCheck->to)break;

    ret = 0;
  }while(0);

  return ret;
}

int MostRecentWithLetter(char * name,tRuleRule * rule)
{

  int ret = 0;
  //Check if this rule has an Letter
  if(strcmp(rule->letters,"-") && !strcmp(rule->save, "0"))
  {
    tRule * fromHash = NULL;
    fromHash = GetRule(name);
    if(fromHash == NULL)
    {
      ret = 1;
    }
    else
    {
      GSList * listEl = g_slist_find_custom(fromHash->list,newRule,MostRecentRule);
      if(listEl == NULL)
      {
        RemoveDeprecatedRules(fromHash);
        ret = 1;
      }
    }
  }

  return ret;
}


int RuleActual(char * name,tRuleRule *rule)
{
  time_t t = time(NULL);
  struct tm now;
  struct tm to;

  localtime_r(&t,&now);
  localtime_r(&rule->to,&to);
  if(to.tm_year+1 < now.tm_year)
  {
    return MostRecentWithLetter(name, rule);
  }
  else
  {
    if(strcmp(rule->letters,"-") && !strcmp(rule->save, "0"))
    {
      tRule * fromHash = NULL;
      fromHash = GetRule(name);
      if(fromHash != NULL)
      {
        RemoveDeprecatedRules(fromHash);
      }
    }
    return 1;
  }
}

void ReadRules(const char * fileName, void * arg)
{
  (void)arg;
  FILE * fp;
  fp = fopen(fileName,"r");
  if(fp != NULL)
  {
    char * line= NULL;
    size_t szLine = 0;
    do{
      if(getline(&line,&szLine,fp) >= 0)
      {
        if(IsRule(line,szLine))
        {
          tRuleRule * rule = NULL;
          char *name = NULL;
          //printf("%s",line);
          name = parseRule(&rule,line,szLine);
          if(RuleActual(name, rule))
          {
            //printRule(name,rule);
            GetOrSetRule(name,rule);
          }
          else
          {
            //dbgPrintRule(rule, name);
            freeRule(rule);
          }
          free(name);
        }

        free(line);
        line = NULL;
        szLine = 0;
      }
    }while(!feof(fp));
    fclose(fp);
  }
}

int IsZone(char * line,size_t szLine)
{
  const char indicator1[] = "Zone";
  if(szLine < strlen(indicator1))return 0;
  if(strncmp(line,indicator1,strlen(indicator1)))return 0;

  return 1;
}

char * mystrtok_r(char * str, char ** save_ptr)
{
  static char buf[512];
  int i;
  int start = 0;
  char * p;
  int quoutes = 0;
  if(str != NULL)
  {
    *save_ptr = str;
  }
  p = *save_ptr;
  i = 0;
  while(*p != 0)
  {
    if(quoutes)
    {
      if(*p == '"')
      {
        quoutes--;
      }
    }
    if(isblank(*p) && !quoutes)
    {
      if(start == 0)
      {
        p++;
        continue;
      }
      buf[i]=0;
      break;
    }
    else
    {
      start = 1;
    }
    if(i == 0 && *p == '"')
    {
      quoutes++;
    }
    if(*p=='\n')
    {
      buf[i++] = 0;
    }
    else
    {
      buf[i++] = *p;
    }
    p++;
  }
  *save_ptr = p;
  buf[i]=0;
  if(*p == 0 && i == 0)
  {
    return NULL;
  }
  return buf;
}

void freeSubZone(tSubZone * sZone)
{
  free(sZone->format);
  free(sZone->gmtoff);
  free(sZone->rule);
  free(sZone->save);
  free(sZone->sUntil);
  free(sZone);
}

time_t ParseUntil(char * ptr, char ** save_ptr)
{
  struct tm tmTmp;



  ZEROTIME(tmTmp);
  tmTmp.tm_year = atoi(ptr)-1900;
  do{
    ptr = mystrtok_r(NULL, save_ptr);
    if(ptr == NULL)break;
    if(!strcmp(ptr,"Jan"))tmTmp.tm_mon = 0;
    else if(!strcmp(ptr,"Feb"))tmTmp.tm_mon = 1;
    else if(!strcmp(ptr,"Mar"))tmTmp.tm_mon = 2;
    else if(!strcmp(ptr,"Apr"))tmTmp.tm_mon = 3;
    else if(!strcmp(ptr,"May"))tmTmp.tm_mon = 4;
    else if(!strcmp(ptr,"Jun"))tmTmp.tm_mon = 5;
    else if(!strcmp(ptr,"Jul"))tmTmp.tm_mon = 6;
    else if(!strcmp(ptr,"Aug"))tmTmp.tm_mon = 7;
    else if(!strcmp(ptr,"Sep"))tmTmp.tm_mon = 8;
    else if(!strcmp(ptr,"Oct"))tmTmp.tm_mon = 9;
    else if(!strcmp(ptr,"Nov"))tmTmp.tm_mon = 10;
    else if(!strcmp(ptr,"Dec"))tmTmp.tm_mon = 11;
    else if(ptr[0] == '#')tmTmp.tm_mon = 0;
    else
    {
      fprintf(stderr,"error unknown month! \"%s\"\n",ptr);
      exit(EXIT_FAILURE);
    }
    //year an month might be enough
  }while(0);



  return mktime(&tmTmp);
}

tSubZone * ParseSubZone(char * ptr, char ** save_ptr)
{
  tSubZone * sZone = NULL;
  tRule * rule = NULL;

  sZone = malloc(sizeof(tSubZone));
  sZone->format = NULL;
  sZone->gmtoff = NULL;
  sZone->rule = NULL;
  sZone->save = NULL;
  sZone->until = 0;
  sZone->sUntil = NULL;

  //printf("%s\t",ptr);
  sZone->gmtoff = strdup(ptr);


  ptr = mystrtok_r(NULL, save_ptr);
  //printf("\nfind RULE : %s ",ptr);
  rule = GetRule(ptr);
  if(rule != NULL)
  {
    //printf("OK\n");
  }
  else
  {
    //printf("FALSE");
  }
  if(!strcmp(ptr,"-"))
  {
    sZone->rule = NULL;
    sZone->save = strdup(ptr);
  }
  else if(rule != NULL)
  {
    sZone->rule = rule;
    sZone->save = NULL;
  }
  else
  {
    if(   (ptr[0]=='-' && isdigit(ptr[1]))
       || (isdigit(ptr[0])))
    {
      sZone->rule = NULL;
      sZone->save = strdup(ptr);
    }
    else
    {
     // freeSubZone(sZone);
     // return NULL;
      sZone->rule = NULL;
      sZone->save = NULL;
    }
  }
  ptr = mystrtok_r(NULL, save_ptr);
  //printf("%s\t",ptr);
  sZone->format = strdup(ptr);
  ptr = mystrtok_r(NULL, save_ptr);
  if(ptr != NULL && *ptr != '#')
  {
    //printf("%s\t",ptr);
    sZone->sUntil = malloc(strlen(ptr) + 1 + strlen(*save_ptr) +1);
    sprintf(sZone->sUntil,"%s%s",ptr,*save_ptr);
    //printf("%s\t",sZone->sUntil);
    sZone->until = ParseUntil(ptr,save_ptr);
  }
  return sZone;
}

int ValidSubZone(tSubZone * sZone)
{
  time_t t;
  struct tm now;
  struct tm to;

  if(sZone->until == 0)
  {
    return 1;
  }

  t = time(NULL);
  localtime_r(&t,&now);
  localtime_r(&sZone->until,&to);
  if(to.tm_year+1 < now.tm_year)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int ParseZone(tZone ** ppZone, char * line, size_t szLine)
{
  char * pLine = line;
  tZone * zone = *ppZone;
  tSubZone * sZone= NULL;
  char * save_ptr;
  char * ptr;
  size_t i;

  for(i=0; i < szLine;i++)
  {
    if(line[i] == '\n')
    {
      line[i] = '\0';
    }
  }


  while(((size_t)(pLine-line)>szLine ) && isblank(*pLine))pLine++;

  //FEHLER???
  if(((size_t)(pLine-line)>szLine ))return 0;

  ptr = mystrtok_r(pLine, &save_ptr);
  //printf("%s\t",ptr);
  if(zone == NULL)
  {
    zone = malloc(sizeof(tZone));
    zone->list = NULL;
    zone->name = NULL;
    ptr = mystrtok_r(NULL, &save_ptr);
    //printf("%s\t",ptr);
    zone->name = strdup(ptr);
    *ppZone = zone;
    g_hash_table_insert(zoneHash,zone->name,zone);
    //printf("%s\n",zone->name);
    //second strtok
    ptr = mystrtok_r(NULL, &save_ptr);
  }
  else
  {
    //printf("\t");
  }
  sZone = ParseSubZone(ptr,&save_ptr);
  //printf("\n");
  if(sZone != NULL)
  {
    if(ValidSubZone(sZone))
    {
      zone->list = g_slist_append(zone->list, sZone);
    }
    if(sZone->until)
    {
      return 1;
    }
  }
  return 0;
}

int isComment(char * line, size_t szLine)
{
  size_t i;
  for(i = 0; i < szLine; i++)
  {
    if(!isblank(line[i]))
    {
      if(line[i]=='#')return 1;
      else return 0;
    }
  }
  return 0;
}
int isLink(char * line,size_t szLine)
{
  const char indicator1[] = "Link";
  if(szLine < strlen(indicator1))return 0;
  if(strncmp(line,indicator1,strlen(indicator1)))return 0;

  return 1;
}


void ReadZones(const char * fileName, void * arg)
{
  (void)arg;
    FILE * fp;
    int cont = 0;
    fp = fopen(fileName,"r");
    if(fp != NULL)
    {
      char * line= NULL;
      size_t szLine = 0;
      tZone    * zone = NULL;
      do{
        if(getline(&line,&szLine,fp) >= 0)
        {
          //tSubZone * sZone= NULL;
          //char *name = NULL;
          //printf("%s",line);
          if(isLink(line,szLine))
          {
            linkList = g_slist_append(linkList, strdup(line));
          }
          else if(!isComment(line,szLine))
          {
            //printf("%s",line);
            switch(cont)
            {
              case 0:
                if(IsZone(line,szLine))
                {
                  //printf("%s",line);
                  if(ParseZone(&zone,line,szLine))
                  {
                    cont++;
                  }
                  else
                  {
                    //puts("single rule");
                  }
                }
                else
                {
                  //puts("noZone");
                }
                break;
              case 1:
                //printf("%s",line);
                if(!ParseZone(&zone,line,szLine))
                {
                  //puts("zone End");
                  cont=0;
                }
                break;
              default:
                cont = 0;
                break;
            }
            if(cont == 0)
            {
              zone = NULL;
            }
          }
          else
          {
            //puts("comment");
          }

          free(line);
          line = NULL;
          szLine = 0;
        }
      }while(!feof(fp));
      fclose(fp);
    }
}

void printRule(gpointer data, gpointer user_data)
{
  tRuleRule * rule = (tRuleRule*)data;
  tPrintRule * ph = (tPrintRule*)user_data;
  char buf[64];
  fprintf(ph->fp,"Rule\t");
  fprintf(ph->fp,"%s\t",ph->name);
  strftime(buf,sizeof(buf),"%Y",localtime(&rule->from));
  fprintf(ph->fp,"%s\t",buf);
  if(rule->to == INT_MAX)
  {
   fprintf(ph->fp,"max\t");
  }
  else if(rule->from == rule->to)
  {
    fprintf(ph->fp,"only\t");
  }
  else
  {
    strftime(buf,sizeof(buf),"%Y",localtime(&rule->to));
    fprintf(ph->fp,"%s\t",buf);
  }
  fprintf(ph->fp,"%s\t",rule->type);
  fprintf(ph->fp,"%s\t",rule->in);
  fprintf(ph->fp,"%s\t",rule->on);
  fprintf(ph->fp,"%s\t",rule->at);
  fprintf(ph->fp,"%s\t",rule->save);
  fprintf(ph->fp,"%s\n",rule->letters);
}

void printAllSubRules(gpointer key, gpointer value, gpointer user_data)
{
  (void)key;
  tRule * rule = (tRule*)value;
  tPrintRule printHelper;
  printHelper.fp = (FILE*) user_data;
  printHelper.name = rule->name;
  g_slist_foreach(rule->list,printRule,&printHelper);
}

void printZone(gpointer data, gpointer user_data)
{
  tSubZone * zone = (tSubZone*)data;
  tPrintRule * ph = (tPrintRule*)user_data;
  char * format = NULL;
  //char buf[64];
  //sprintf(format,"%s",zone->format);
  format = strdup(zone->format);
  ph->lastSub = zone;
  if(ph->counter != 0)
  {
    fprintf(ph->fp,"\t\t\t");
    ph->counter ++;
  }
  fprintf(ph->fp,"%s\t",zone->gmtoff);
  if(zone->rule != NULL)
  {
    fprintf(ph->fp,"%s\t",zone->rule->name);
  }
  else if(zone->save != NULL)
  {
    fprintf(ph->fp,"%s\t",zone->save);
  }
  else
  {
    int i,j;

    fprintf(ph->fp,"-\t");
    j = 0;
    for(i=0;zone->format[i]!=0;i++)
    {
      if(zone->format[i]=='%')
      {
        //the other ++ copmes from the loop
        i+=1;
      }
      else
      {
        format[j++]=zone->format[i];
      }
    }
    format[j]=0;
    if(strlen(format) == 0)
    {
      sprintf(format, "GMT");
    }

  }

  fprintf(ph->fp,"%s\t",format);

  free(format);
  if(zone->sUntil != NULL)
  {
    fprintf(ph->fp,"%s\t",zone->sUntil);
  }
  fprintf(ph->fp,"\n");
}

void printLastZone(gpointer data, gpointer user_data)
{
  tSubZone * zone = (tSubZone*)data;
  tPrintRule * ph = (tPrintRule*)user_data;
  char * format = NULL;
  //char buf[64];
  //sprintf(format,"%s",zone->format);
  format = strdup(zone->format);
  ph->lastSub = zone;
  if(ph->counter != 0)
  {
    fprintf(ph->fp,"\t\t\t");
    ph->counter ++;
  }
  fprintf(ph->fp,"%s\t",zone->gmtoff);
  if(zone->rule != NULL)
  {
    fprintf(ph->fp,"%s\t",zone->rule->name);
  }
  else if(zone->save != NULL)
  {
    fprintf(ph->fp,"%s\t",zone->save);
  }
  else
  {
    int i,j;

    fprintf(ph->fp,"-\t");
    j = 0;
    for(i=0;zone->format[i]!=0;i++)
    {
      if(zone->format[i]=='%')
      {
        //the other ++ copmes from the loop
        i+=1;
      }
      else
      {
        format[j++]=zone->format[i];
      }
    }
    format[j]=0;
    if(strlen(format) == 0)
    {
      sprintf(format, "GMT");
    }

  }

  fprintf(ph->fp,"%s\t",format);

  free(format);
 /* if(zone->sUntil != NULL)
  {
    fprintf(ph->fp,"%s\t",zone->sUntil);
  }*/
  fprintf(ph->fp,"\n");
}

void printZoneTest(gpointer data, gpointer user_data)
{
  tSubZone * zone = (tSubZone*)data;
  tPrintRule * ph = (tPrintRule*)user_data;
  //in the last loop, the canPrint must be 1
  ph->canPrint=1;
  if(zone->sUntil != NULL)
  {
    ph->canPrint=0;
  }

}

void printAllSubZones(gpointer key, gpointer value, gpointer user_data)
{
  (void)key;
  tZone * zone = (tZone*)value;
  static tPrintRule printHelper;
  printHelper.fp = (FILE*) user_data;
  printHelper.name = zone->name;
  printHelper.counter = 0;
  //fprintf(printHelper.fp,"%s\n",zone->name);

  if(zone->list != NULL)
  {
    printHelper.canPrint = 0;
    //g_slist_foreach(zone->list,printZoneTest,&printHelper);
    //if(printHelper.canPrint)
    {
      printHelper.lastSub = NULL;
      fprintf(printHelper.fp,"Zone\t");
      fprintf(printHelper.fp,"%s\t",printHelper.name);
      g_slist_foreach(zone->list,printZone,&printHelper);
      if(printHelper.lastSub->sUntil != NULL)
      {
        printLastZone(printHelper.lastSub,&printHelper);
      }
      fprintf(printHelper.fp,"\n\n");
    }
  }
}


void printRules(void)
{
  FILE * fp = strippedout;

  g_hash_table_foreach(ruleHash,printAllSubRules,fp);
}

void printZones(void)
{
  FILE * fp = strippedout;

  g_hash_table_foreach(zoneHash,printAllSubZones,fp);
}

void printLink(gpointer data, gpointer user_data)
{
  FILE * fp = (FILE*)user_data;
  char * link = (char*)data;
  fprintf(fp,"%s",link);
}

void printLinks(void)
{
  FILE * fp = strippedout;

  g_slist_foreach(linkList,printLink,fp);
}

int main(int argc, char ** argv)
{
  char * source = NULL;
  //DIR * dirSource;
  //struct dirent *dir;
  char * dest = NULL;
  char strippedfile[4096];
  struct stat s = {0};
  ruleHash = g_hash_table_new(g_str_hash,g_str_equal);
  zoneHash = g_hash_table_new(g_str_hash,g_str_equal);

  if(argc != 3)
  {
    fprintf(stderr,"usage: tzstrip <sourcedir> <destdir>\n");
    exit(EXIT_FAILURE);
  }



  source = argv[1];
  if (stat(source, &s))
  {
    if( errno == ENOENT )
    {
      mkdir(source,0777);
    }
    else
    {
      perror("stat()");
      exit(EXIT_FAILURE);
    }
  }
  if(!(s.st_mode && S_IFDIR))
  {
    fprintf(stderr,"error directory \"%s\" does not exist!\n",source);
    exit(EXIT_FAILURE);
  }


  dest   = argv[2];
  if (stat(dest, &s))
  {
    if( errno == ENOENT )
    {
      mkdir(dest,0777);
    }   
    else
    {
      perror("stat()");
      exit(EXIT_FAILURE);
    }
  }
  if(!(s.st_mode && S_IFDIR))
  {
    fprintf(stderr,"error directory \"%s\" does not exist!\n",dest);
    exit(EXIT_FAILURE);
  }

  IterDir(source,ReadRules, dest);
  IterDir(source,ReadZones, dest);

  sprintf(strippedfile,"%s/tzstripped",dest);

  strippedout = fopen(strippedfile,"w");

  printRules();
  printZones();
  printLinks();

  fprintf(strippedout,"#End\n");

  fclose(strippedout);
  //puts("fertig");
  return 0;
}
