/*
Student Name: Indeep Farma
Student ID: 1015266
Student Email: ifarma@uoguelph.ca
Assignment 2 CIS 2750
*/

#include <stdio.h>
#include "VCParser.h"
#include "LinkedListAPI.h"
#include <ctype.h>
#include "VCHelpers.h"

/* Function parses and allocates a Card object*/
VCardErrorCode createCard(char* fileName, Card** newCardObject) {

  /* Check for NULL pointer for filename */
  if(fileName == NULL) {
    return INV_FILE;
  }

  /* Declaring variables */
  bool hasBegin = false;
  bool hasEnd = false;
  bool hasFnProp = false;
  bool hasVersion = false;
  Card *tmpCard = malloc(sizeof(Card));
  FILE *fp = NULL;
  char ch = ' ';
  char *version = NULL;
  char *fileNameBuffer = malloc(strlen(fileName) * sizeof(char) + 1);
  char *lineBuffer = NULL;
  char *property = NULL;
  char *vcardBuffer = NULL;
  int bCount = 0;
  int eCount = 0;
  int vCount = 0;
  int clLength = 0;
  int fileLen = 0;
  int i = 0;

  char *tempString = NULL;
  char *timeStr = NULL;
  char *dateStr = NULL;

  /*Copy file name into buffer so strtok doesnt mess it up*/
  strcpy(fileNameBuffer, fileName);

  /* Open the file */
  fp = fopen(fileNameBuffer, "r");

  if(fp == NULL) {
    free(fileNameBuffer);
    free(tmpCard);
    return INV_FILE;
  } else if(isValidExtention(fileNameBuffer)) {

    /*Retrieve size of file*/
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    vcardBuffer = malloc(sizeof(char) * fileLen + 1);

    /* Copy contents of the file into memory */
    do{
      ch = fgetc(fp);
      if(i < fileLen) {
        vcardBuffer[i++] = ch;
      }
    }
    while(!feof(fp));

    vcardBuffer[i] = '\0';

    /************************************ GET THE CONTENT LINE ********************************/
    i = 0;
    int *p = &i;
    char *group = NULL;

    /* Loop through the entire thing to make sure that there is BEGIN, VERSION, and END */
    while(vcardBuffer[i] != '\0') {
      clLength = getContentLineLength(vcardBuffer, p);
      lineBuffer = getLine(vcardBuffer, p, clLength);
      property = getProperty(lineBuffer, clLength);

      if(property == NULL) {

         /* Make sure to free everything before return */
        if(fp != NULL) {fclose(fp);}
        if(fileNameBuffer != NULL) {free(fileNameBuffer);}
        if(vcardBuffer != NULL) {free(vcardBuffer);}
        if(lineBuffer != NULL) {free(lineBuffer);}
        if(property != NULL) {free(property);}
        if(group != NULL) {free(group);}
        
        if(tmpCard != NULL) {
          free(tmpCard);
          tmpCard = NULL;
        }

        *newCardObject = tmpCard;

        return INV_CARD; //only way it returns null is if theres no colon del
      }

      if(strcmpCI(property, "BEGIN") == 0) {
        hasBegin = true;
        bCount++;
      }

      if(strcmpCI(property, "VERSION") == 0) {
        hasVersion = true;
        vCount++;
        version = getStringRightColon(lineBuffer, clLength);
      }

      if(strcmpCI(property, "END") == 0) {
        hasEnd = true;
        eCount++;
      }

      /* Free to make sure all instances were freed*/
      if(lineBuffer != NULL) {
        free(lineBuffer);
        lineBuffer = NULL;  
      }

      if(property != NULL) {
        free(property);
        property = NULL;  
      }
    }

    /* Make sure has the required properties */
    if(hasBegin == false || hasVersion == false || hasEnd == false || bCount > 1 || vCount > 1 || eCount > 1 ||
      strcmp(version, "4.0") != 0) {

      free(version);

      if(fp != NULL) {fclose(fp);}
      if(fileNameBuffer != NULL) {free(fileNameBuffer);}
      if(vcardBuffer != NULL) {free(vcardBuffer);}

      if(tmpCard != NULL) {
        free(tmpCard);
        tmpCard = NULL;
      }

      *newCardObject = tmpCard;
      
      return INV_CARD;
    }

    /*Reset the variables */
    if(version != NULL){
      free(version); 
      version = NULL;
    }

    i = 0;
    p = &i;
    clLength = 0;

    Property *tmpProp = NULL;

    tmpCard->fn = NULL;
    tmpCard->birthday = NULL;
    tmpCard->anniversary = NULL;
    tmpCard->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);

    char* rightStr = NULL;

    /* Get the line content and parse it */
    while(vcardBuffer[i] != '\0') {

      clLength = getContentLineLength(vcardBuffer, p);
      lineBuffer = getLine(vcardBuffer, p, clLength);
      property = getProperty(lineBuffer, clLength);
      group = getGroup(lineBuffer, clLength);

      if(property == NULL) {

         /* Make sure to free everything before return */
        if(fp != NULL) {fclose(fp);}
        if(fileNameBuffer != NULL) {free(fileNameBuffer);}
        if(vcardBuffer != NULL) {free(vcardBuffer);}
        if(lineBuffer != NULL) {free(lineBuffer);}
        if(property != NULL) {free(property);}
        if(group != NULL) {free(group);}
        
        if(tmpCard->fn != NULL) {
          deleteProperty(tmpCard->fn);
        }
        if(tmpCard->birthday != NULL) {
          deleteDate(tmpCard->birthday);
        }
        if(tmpCard->anniversary != NULL) {
          deleteDate(tmpCard->anniversary);
        }
        if(tmpCard->optionalProperties != NULL) {
          freeList(tmpCard->optionalProperties);
        }
        if(tmpCard != NULL) {
          free(tmpCard);
          tmpCard = NULL;
        }

        *newCardObject = tmpCard;

        return INV_CARD; //only way it returns null is if theres no colon del
      }

      /* Check if the property is valid */
      if(isValidProperty(property) == false) {

        /* Make sure to free everything before return */
        if(fp != NULL) {fclose(fp);}
        if(fileNameBuffer != NULL) {free(fileNameBuffer);}
        if(vcardBuffer != NULL) {free(vcardBuffer);}
        if(lineBuffer != NULL) {free(lineBuffer);}
        if(property != NULL) {free(property);}
        if(group != NULL) {free(group);}
        
        if(tmpCard->fn != NULL) {
          deleteProperty(tmpCard->fn);
        }
        if(tmpCard->birthday != NULL) {
          deleteDate(tmpCard->birthday);
        }
        if(tmpCard->anniversary != NULL) {
          deleteDate(tmpCard->anniversary);
        }
        if(tmpCard->optionalProperties != NULL) {
          freeList(tmpCard->optionalProperties);
        }
        if(tmpCard != NULL) {
          free(tmpCard);
          tmpCard = NULL;
        }

        *newCardObject = tmpCard;

        return INV_PROP;
      }

      /* Dont need BEGIN or VERSION properties in struct */
      if(strcmpCI(property, "BEGIN") == 0 || strcmpCI(property, "VERSION") == 0) {
        if(lineBuffer != NULL) {free(lineBuffer);}
        if(property != NULL) {free(property);}
        if(group != NULL) {free(group);}
        continue;
      }

      /* Placing FN property in the vcard struct */
      if(strcmpCI(property, "FN") == 0 && hasFnProp == false) {
        hasFnProp = true;

        tmpCard->fn = malloc(sizeof(Property));
        tmpCard->fn->name = malloc(sizeof(char) * strlen(property) + 1);

        tmpCard->fn->group = malloc(sizeof(char) * strlen(group) + 1);

        strcpy(tmpCard->fn->name, property);
        strcpy(tmpCard->fn->group, group);
        // printf("TEMP->FN->NAME = %s\n", tmpCard->fn->name);
        // printf("TEMP->FN->GROUP = %s\n", tmpCard->fn->group);

        tmpCard->fn->parameters = getParametersList(lineBuffer, clLength);

        tmpCard->fn->values = getValuesList(lineBuffer, clLength);

        /*if invalid we need to free*/
        if(tmpCard->fn->parameters == NULL || tmpCard->fn->values == NULL) {

          deleteProperty(tmpCard->fn);

          if(lineBuffer != NULL) {free(lineBuffer);}
          if(property != NULL) {free(property);}
          if(group != NULL) {free(group);}
          if(fp != NULL) {fclose(fp);}
          if(fileNameBuffer != NULL) {free(fileNameBuffer);}
          if(vcardBuffer != NULL) {free(vcardBuffer);}
       
          if(tmpCard->birthday != NULL) {
            deleteDate(tmpCard->birthday);
          }
          if(tmpCard->anniversary != NULL) {
            deleteDate(tmpCard->anniversary);
          }
          if(tmpCard->optionalProperties != NULL) {
            freeList(tmpCard->optionalProperties);
          }
          if(tmpCard != NULL) {
            free(tmpCard);
            tmpCard = NULL;
          }

          *newCardObject = tmpCard;
          
          return INV_PROP;
        }

        /* CHECK TO SEE WHAT IS INSIDE THE LIST OF PARAMETERS */
        // void *elem;
        // ListIterator iter = createIterator(tmpCard->fn->values);
        // while((elem = nextElement(&iter)) != NULL) {
        //   char *one = (char*)elem;
        //   char *string = tmpCard->fn->values->printData(one);
        //   printf("%s\n", string);
        //   // free(string);
        // }
      }
      /* If there is the birthday property fill that struct */
      else if(strcmpCI(property, "BDAY") == 0) {
        tmpCard->birthday = malloc(sizeof(DateTime));
        tmpCard->birthday->text = NULL;
        tmpCard->birthday->date = NULL;
        tmpCard->birthday->time = NULL;

        tempString = NULL;
        timeStr = NULL;
        dateStr = NULL;

        rightStr = getStringRightColon(lineBuffer, clLength);

        //Free an return invalid property
        if(rightStr == NULL) {
          if(lineBuffer != NULL) {free(lineBuffer);}
          if(property != NULL) {free(property);}
          if(group != NULL) {free(group);}
          if(fp != NULL) {fclose(fp);}
          if(fileNameBuffer != NULL) {free(fileNameBuffer);}
          if(vcardBuffer != NULL) {free(vcardBuffer);}
      
          if(tmpCard->fn != NULL) {
            deleteProperty(tmpCard->fn);
          }
          if(tmpCard->birthday != NULL) {
            deleteDate(tmpCard->birthday);
          }
          if(tmpCard->anniversary != NULL) {
            deleteDate(tmpCard->anniversary);
          }
          if(tmpCard->optionalProperties != NULL) {
            freeList(tmpCard->optionalProperties);
          }
          if(tmpCard != NULL) {
            free(tmpCard);
            tmpCard = NULL;
          }

          *newCardObject = tmpCard;

          return INV_PROP;
        }

        if(isalpha(rightStr[0]) != 0 && isalpha(rightStr[1]) != 0) {
          tmpCard->birthday->UTC = false;
          tmpCard->birthday->isText = true;
        } else if(rightStr[strlen(rightStr) - 1] == 'Z') {
          tmpCard->birthday->UTC = true;
          tmpCard->birthday->isText = false;
        } else {
          tmpCard->birthday->UTC = false;
          tmpCard->birthday->isText = false;
        }

        /* Copy value into struct */
        if(tmpCard->birthday->isText == true) {

          tmpCard->birthday->text = malloc(sizeof(char) * strlen(rightStr) + 2);
          tmpCard->birthday->date = malloc(sizeof(char) + 2);
          tmpCard->birthday->time = malloc(sizeof(char) + 2);
          tempString = malloc(sizeof(char) + 2);
          tempString[0] = '\0';

          strcpy(tmpCard->birthday->text, rightStr);
          strcpy(tmpCard->birthday->date, tempString);
          strcpy(tmpCard->birthday->time, tempString);
        } 
        else {

          if(hasTime(rightStr)) {
            if(rightStr[0] == 'T') {
              dateStr = malloc(sizeof(char) + 2);
              dateStr[0] = '\0';
            } else {
              dateStr = getDateString(rightStr);
            }
            timeStr = getTimeString(rightStr);

            tmpCard->birthday->date = malloc(sizeof(char) * strlen(dateStr) + 2);
            tmpCard->birthday->time = malloc(sizeof(char) * strlen(timeStr) + 2);
            tmpCard->birthday->text = malloc(sizeof(char) + 2);
            tempString = malloc(sizeof(char) + 2);
            tempString[0] = '\0';

            strcpy(tmpCard->birthday->date, dateStr);
            strcpy(tmpCard->birthday->time, timeStr);
            strcpy(tmpCard->birthday->text, tempString); 

          } else {

            /* Malloc/Init */
            tmpCard->birthday->date = malloc(sizeof(char) * strlen(rightStr) + 2);
            tmpCard->birthday->time = malloc(sizeof(char) + 2);
            tmpCard->birthday->text = malloc(sizeof(char) + 2);
            tempString = malloc(sizeof(char) + 2);
            tempString[0] = '\0';

            strcpy(tmpCard->birthday->date, rightStr);
            strcpy(tmpCard->birthday->time, tempString);
            strcpy(tmpCard->birthday->text, tempString);
          }
        }

        /* Free right string */
        if(rightStr != NULL) {
          free(rightStr);
          rightStr = NULL;
        }
        if(tempString != NULL) {
          free(tempString);
          tempString = NULL;
        }
        if(timeStr != NULL) {
          free(timeStr);
          timeStr = NULL;
        }
        if(dateStr != NULL) {
          free(dateStr);
          dateStr = NULL;
        }

      }
      /* If there is an anniversary property fill the struct */
      else if(strcmpCI(property, "ANNIVERSARY") == 0) {
        tmpCard->anniversary = malloc(sizeof(DateTime));
        tmpCard->anniversary->text = NULL;
        tmpCard->anniversary->date = NULL;
        tmpCard->anniversary->time = NULL;

        tempString = NULL;
        timeStr = NULL;
        dateStr = NULL;

        rightStr = getStringRightColon(lineBuffer, clLength);

        if(rightStr == NULL) {
          if(lineBuffer != NULL) {free(lineBuffer);}
          if(property != NULL) {free(property);}
          if(group != NULL) {free(group);}
          if(fp != NULL) {fclose(fp);}
          if(fileNameBuffer != NULL) {free(fileNameBuffer);}
          if(vcardBuffer != NULL) {free(vcardBuffer);}
      
          if(tmpCard->fn != NULL) {
            deleteProperty(tmpCard->fn);
          }
          if(tmpCard->birthday != NULL) {
            deleteDate(tmpCard->birthday);
          }
          if(tmpCard->anniversary != NULL) {
            deleteDate(tmpCard->anniversary);
          }
          if(tmpCard->optionalProperties != NULL) {
            freeList(tmpCard->optionalProperties);
          }
          if(tmpCard != NULL) {
            free(tmpCard);
            tmpCard = NULL;
          }

          *newCardObject = tmpCard;

          return INV_PROP;
        }

        if(isalpha(rightStr[0]) != 0 && isalpha(rightStr[1]) != 0) {
          tmpCard->anniversary->UTC = false;
          tmpCard->anniversary->isText = true;
        } else if(rightStr[strlen(rightStr) - 1] == 'Z') {
          tmpCard->anniversary->UTC = true;
          tmpCard->anniversary->isText = false;
        } else {
          tmpCard->anniversary->UTC = false;
          tmpCard->anniversary->isText = false;
        }

        /* Copy value into struct */
        if(tmpCard->anniversary->isText == true) {

          tmpCard->anniversary->text = malloc(sizeof(char) * strlen(rightStr) + 2);
          tmpCard->anniversary->date = malloc(sizeof(char) + 2);
          tmpCard->anniversary->time = malloc(sizeof(char) + 2);
          tempString = malloc(sizeof(char) + 2);
          tempString[0] = '\0';

          strcpy(tmpCard->anniversary->text, rightStr);
          strcpy(tmpCard->anniversary->date, tempString);
          strcpy(tmpCard->anniversary->time, tempString);
        } 
        else {

          if(hasTime(rightStr)) {
            if(rightStr[0] == 'T') {
              dateStr = malloc(sizeof(char) + 2);
              dateStr[0] = '\0';
            } else {
              dateStr = getDateString(rightStr);
            }
            timeStr = getTimeString(rightStr);

            tmpCard->anniversary->date = malloc(sizeof(char) * strlen(dateStr) + 2);
            tmpCard->anniversary->time = malloc(sizeof(char) * strlen(timeStr) + 2);
            tmpCard->anniversary->text = malloc(sizeof(char) + 2);
            tempString = malloc(sizeof(char) + 2);
            tempString[0] = '\0';

            strcpy(tmpCard->anniversary->date, dateStr);
            strcpy(tmpCard->anniversary->time, timeStr);
            strcpy(tmpCard->anniversary->text, tempString); 

          } else {

            /* Malloc/Init */
            tmpCard->anniversary->date = malloc(sizeof(char) * strlen(rightStr) + 2);
            tmpCard->anniversary->time = malloc(sizeof(char) + 2);
            tmpCard->anniversary->text = malloc(sizeof(char) + 2);
            tempString = malloc(sizeof(char) + 2);
            tempString[0] = '\0';

            strcpy(tmpCard->anniversary->date, rightStr);
            strcpy(tmpCard->anniversary->time, tempString);
            strcpy(tmpCard->anniversary->text, tempString);
          }
        }

         /* Free right string */
        if(rightStr != NULL) {
          free(rightStr);
          rightStr = NULL;
        }
        if(tempString != NULL) {
          free(tempString);
          tempString = NULL;
        }
        if(timeStr != NULL) {
          free(timeStr);
          timeStr = NULL;
        }
        if(dateStr != NULL) {
          free(dateStr);
          dateStr = NULL;
        }
      }
      else {
        if(strcmpCI(property, "END") != 0) {
          tmpProp = malloc(sizeof(Property));
          tmpProp->name = malloc(sizeof(char) * strlen(property) + 2);
          tmpProp->group = malloc(sizeof(char) * strlen(group) + 2);

          strcpy(tmpProp->name, property);
          strcpy(tmpProp->group, group);

          tmpProp->parameters = getParametersList(lineBuffer, clLength - 1);
          tmpProp->values = getValuesList(lineBuffer, clLength - 1);

          if(tmpProp->parameters == NULL || tmpProp->values == NULL) {

            if(lineBuffer != NULL) {free(lineBuffer);}
            if(property != NULL) {free(property);}
            if(group != NULL) {free(group);}
            if(fp != NULL) {fclose(fp);}
            if(fileNameBuffer != NULL) {free(fileNameBuffer);}
            if(vcardBuffer != NULL) {free(vcardBuffer);}
        
            if(tmpCard->fn != NULL) {
              deleteProperty(tmpCard->fn);
            }
            if(tmpCard->birthday != NULL) {
              deleteDate(tmpCard->birthday);
            }
            if(tmpCard->anniversary != NULL) {
              deleteDate(tmpCard->anniversary);
            }
            if(tmpCard->optionalProperties != NULL) {
              freeList(tmpCard->optionalProperties);
            }
            if(tmpCard != NULL) {
              free(tmpCard);
              tmpCard = NULL;
            }
            if(tmpProp != NULL) {
              deleteProperty(tmpProp);
            }

            *newCardObject = tmpCard;
            
            return INV_PROP;
          }

          insertBack(tmpCard->optionalProperties, (void*)tmpProp);

        }
      }

      /* Free to make sure all instances were freed*/
      if(lineBuffer != NULL) {free(lineBuffer);}
      if(property != NULL) {free(property);}
      if(group != NULL) {free(group);}
    }

    /* If there is no fn property */
    if(hasFnProp == false) {
      if(tmpCard != NULL) {
        deleteCard(tmpCard);
        tmpCard = NULL;
      }
      if(fp != NULL) {fclose(fp);}
      if(fileNameBuffer != NULL) {free(fileNameBuffer);}
      if(vcardBuffer != NULL) {free(vcardBuffer);}

      *newCardObject = tmpCard;
      return INV_CARD;
    }

    /* Free Statements */
    if(fp != NULL) {fclose(fp);}
    if(fileNameBuffer != NULL) {free(fileNameBuffer);}
    if(vcardBuffer != NULL) {free(vcardBuffer);}

    *newCardObject = tmpCard;

    return OK;
  }

  if(fileNameBuffer != NULL){free(fileNameBuffer);}
  if(fp != NULL) {fclose(fp);}
  if(vcardBuffer != NULL) {free(vcardBuffer);}
  if(lineBuffer != NULL) {free(lineBuffer);}

  if(tmpCard != NULL) {
    free(tmpCard);
    tmpCard = NULL;
  }

  *newCardObject = tmpCard;

  return OTHER_ERROR;
}


/********* HELPER FUNCTIONS ***********/

/* This function does a check for if its the same string
   case insensitive
   I have retrieved this function from: 
   https://stackoverflow.com/questions/5820810/case-insensitive-string-comp-in-c
*/
int strcmpCI(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

/* Does a check that line has a colon
** @param line is the line we are checking
** @param lineLen is the length of the line */
bool isColon(char *line, int lineLen) {
  int i = 0;

  /* Loop through line and see if colon is there */
  for(i = 0; i < lineLen; i++) {
    if(line[i] == ':') {
      return true;
    }
  }

  return false;
}

/* Checks string passed into it if has group deliminater
** @param line is the line we check */
bool isGroupDelim(char *line, int lineLen) {
  int i = 0;

  /* Loop through line and see if semi colon exists*/
  for(i = 0; i < lineLen; i++) {
    if(line[i] == '.') {
      return true;
    }
  }

  return false;
}

/* Does a check that line has a semi colon
** @param line is the line we are checking
** @param lineLen is the length of the line */
bool isSemiColon(char *line, int lineLen) {
  int i = 0;

  /* Loop through line and see if semi colon exists*/
  for(i = 0; i < lineLen; i++) {
    if(line[i] == ';') {
      return true;
    }
  }
  
  return false;
}

/* Checks if the filename extension is valid 
** @param fileName is name of file */
bool isValidExtention(char *fileName) {

  /* Create token for deliminator*/
  char *token = strtok(fileName, ".");
  /* Get extension part of the filename */
  token = strtok(NULL, ".");

  /* Compare extension to make sure it is valid */
  if(strcmp(token, "vcf") == 0) {
    return true;
  } else if(strcmp(token, "vcard") == 0) {
    return true;
  }
 
  return false;
}

/* Checks if the property is a valid property
** @param prop is the property string */
bool isValidProperty(char* prop) {
  if(prop == NULL) {
    return false;
  }
  
  if( strcmpCI(prop, "FN") == 0 ||
      strcmpCI(prop, "SOURCE") == 0 ||
      strcmpCI(prop, "KIND") == 0 ||
      strcmpCI(prop, "N") == 0 ||
      strcmpCI(prop, "NICKNAME") == 0 ||
      strcmpCI(prop, "PHOTO") == 0 ||
      strcmpCI(prop, "BDAY") == 0 ||
      strcmpCI(prop, "ANNIVERSARY") == 0 ||
      strcmpCI(prop, "GENDER") == 0 ||
      strcmpCI(prop, "ADR") == 0 ||
      strcmpCI(prop, "TEL") == 0 ||
      strcmpCI(prop, "EMAIL") == 0 ||
      strcmpCI(prop, "IMPP") == 0 ||
      strcmpCI(prop, "LANG") == 0 ||
      strcmpCI(prop, "TZ") == 0 ||
      strcmpCI(prop, "GEO") == 0 ||
      strcmpCI(prop, "TITLE") == 0 ||
      strcmpCI(prop, "ROLE") == 0 ||
      strcmpCI(prop, "LOGO") == 0 ||
      strcmpCI(prop, "ORG") == 0 ||
      strcmpCI(prop, "MEMBER") == 0 ||
      strcmpCI(prop, "RELATED") == 0 ||
      strcmpCI(prop, "CATEGORIES") == 0 ||
      strcmpCI(prop, "NOTE") == 0 ||
      strcmpCI(prop, "PRODID") == 0 ||
      strcmpCI(prop, "REV") == 0 ||
      strcmpCI(prop, "SOUND") == 0 ||
      strcmpCI(prop, "UID") == 0 ||
      strcmpCI(prop, "CLIENTPIDMA{") == 0 ||
      strcmpCI(prop, "URL") == 0 ||
      strcmpCI(prop, "KEY") == 0 ||
      strcmpCI(prop, "FBURL") == 0 ||
      strcmpCI(prop, "CALADRURI") == 0 ||
      strcmpCI(prop, "CALURI") == 0 ||
      strcmpCI(prop, "XML") == 0 ||
      strcmpCI(prop, "BEGIN") == 0 ||
      strcmpCI(prop, "VERSION") == 0 ||
      strcmpCI(prop, "END") == 0)
      {
        return true;
      }

  return false;
}

/* This function checks if there is a time value in the line given
** @param line is the right side of a contentline */
bool hasTime(char *line) {

  bool isTime = false;
  int i = 0;
  char ch = line[i++];

  while(ch != '\0') {
    if(ch == 'T') {
      isTime = true;
      return isTime;
    }
    ch = line[i++];
  }

  return isTime;
}

/* This function gets the date portion from the UTC in the line given
** @param line is the right side of a contentline */
char *getDateString(char *line) {

  int i = 0;
  int j = 0;
  char ch = line[i];
  char *date = malloc(sizeof(char) + 2);

  while(ch != 'T') {
    ch = line[i++];
    date[j++] = ch;
    date = realloc(date, sizeof(char) * j + 2);
    ch = line[i];
  }

  date[j] = '\0';

  return date;

}

/* This function gets the time portion from the UTC in the line given
** @param line is the right side of a contentline */
char *getTimeString(char *line) {

  int i = 0;
  int j = 0;
  int index = 0;
  char ch = line[i];
  char *time = malloc(sizeof(char) + 2);

  while(ch != 'T') {
    ch = line[i++];
    index++;
  }

  if(line[0] == 'T') {
    index++;
  }

  while(ch != '\0') {
    ch = line[index++];
    time[j++] = ch;
    time = realloc(time, sizeof(char) * j + 2);
    /**** ADDED TO FIX UTC, TAKE OUT IF AFFECTING CODE ***/
    if(line[index] == 'Z')
    {
      break;
    }
  }

  time[j] = '\0';

  return time;

}

/* This function retrieves the group value from the contentline given
** @param line is the contentline
** @param lineLen is the length of the contentLine */
char *getGroup(char *line, int lineLen) {

  /* Declaring variables */
  char *temp = NULL;
  char *group = NULL;
  int charCount = 0;
  int delimIndex = 0;
  int i = 0;


  temp = getStringLeftColon(line, &charCount, lineLen);
  
  if(temp == NULL) {
    return temp;
  }

  /*If theres a group copy it into group buffer */
  if(isGroupDelim(temp, charCount) == true) {
    delimIndex = getGroupDelimIndex(temp, charCount);
    group = malloc(sizeof(char) + 1);
    for(i = 0; i < delimIndex; i++) {
      group[i] = temp[i];
      group = realloc(group, sizeof(char) * i + 2);
    }
    // i++;
    group[i] = '\0';
    free(temp);
  } else {
    /*Otherwise make group buffer empty string */
    group = malloc(sizeof(char) + 1);
    group[0] = '\0';
    free(temp);
  }

  return group;
}

/* This function gets the contentline from the vcard buffer
** @param vcard is the string of the vcard in memory
** @index is the index we're at in the vcard string
** @lineLen is the length of the line we're getting*/
char *getLine(char *vcard, int *index, int lineLen) {

  bool isDone = false;
  int j = 0;
  int i = *index;
  char ch;
  char *line = malloc(sizeof(char) * lineLen + 1);

  /* Get the line even if it has line folding */
  while(isDone != true) {
    while((vcard[i] != '\n')) {
      ch = vcard[i++];
      line[j++] = ch;
    }
    if(vcard[i+1] != ' ') {
      isDone = true;
    } else if(vcard[i+1] == ' '){
      i = i + 2;
      j--; // Get rid of the cr
    }
  }

  i++; //Increment i so that we arent on a \n char
  line[j - 1] = '\0'; //place null char to end string

  *index = i;
  return line;

}

/* This function retrieves the parameter from the line given
** @param line is the line with parameter and its assigned value*/
char *getParameter(char *line) {

  /* Declaring variables */
  bool hasEqual = false;
  char ch = ' ';
  char *param = malloc(sizeof(char));
  int i = 0;
  int j = 0;

  /* Make the parameter an empty string by default */
  param[0] = '\0';

  /*check if has = char */
  while(ch != '\0') {
    ch = line[i++];
    if(ch == '=') {
      hasEqual = true;
    }
  }

  if(hasEqual) {
    free(param);
    param = malloc(sizeof(char) + 2);
    i = 0;
    ch = line[i];

    while(ch != '=') {
      ch = line[i++];
      param[j++] = ch;
      param = realloc(param, sizeof(char) * j + 1);
      ch = line[i];
    }
    param[j] = '\0';
    return param;

  } else {
    return param;
  }

  return param;
}

/* This function retrieves the parameter value from the line given
** @param line is the line with parameter and its assigned value */
char *getParameterValue(char *line) {
   /* Declaring variables */
  bool hasEqual = false;
  char ch = ' ';
  char *value = malloc(sizeof(char));
  int i = 0;
  int j = 0;
  int startIndex;

  /* Make the value an empty string by default */
  value[0] = '\0';
  ch = line[0];

  /*check if has = char */
  while(ch != '\0') {
    ch = line[i++];
    if(ch == '=') {
      hasEqual = true;
      startIndex = i;
    }
  }

  if(hasEqual) {
    free(value);
    value = malloc(sizeof(char) + 2);
    i = startIndex;
    ch = line[i];

    while(ch != '\0') {
      ch = line[i++];
      value[j++] = ch;
      value = realloc(value, sizeof(char) * j + 1);
      ch = line[i];
    }
    value[j] = '\0';

    return value;

  } else {
    return value;
  }

  return value;
}

/* This function retrieves the parameter list from the contentline given
** @param line is the contentline
** @param lineLen is the length of the contentLine */
List *getParametersList(char *line, int lineLen) {
  
  Parameter *tmpParam;
  char ch = ' ';
  char *param = NULL;
  char *temp = NULL;
  char *tmpPnV = NULL;
  char *value = NULL;
  int charCount = 0;
  int index = 0;
  int i = 0;
  int j = 0;
  List *list = initializeList(&parameterToString, &deleteParameter, &compareParameters);

  /* Get the left side of colon from content line */
  temp = getStringLeftColon(line, &charCount, lineLen);

  /* Check if there are any parameters */
  if(isSemiColon(temp, charCount)) {

    tmpPnV = malloc(sizeof(char) + 2);
    index = getIndexSemiColon(temp);

    /* Loop through the line and retrieve all the parameters*/
    for(i = index; i < charCount; i++) {
      ch = temp[i];

      if(ch == ';') {
        tmpPnV[j] = '\0';

        param = getParameter(tmpPnV);
        value = getParameterValue(tmpPnV);

        tmpParam = malloc(sizeof(Parameter));
        tmpParam->name = malloc(sizeof(char) * strlen(param) + 2);
        tmpParam->value = malloc(sizeof(char) * strlen(value) + 2);

        /*If invalid parameter or value free everything */
        if(param[0] == '\0' || value[0] == '\0') {
          free(param);
          free(value);
          if(tmpPnV != NULL) {free(tmpPnV);}
          if(temp != NULL) {free(temp);}

          if(tmpParam->name != NULL) {free(tmpParam->name);}
          if(tmpParam->value != NULL) {free(tmpParam->value);}
          if(tmpParam != NULL) {free(tmpParam);}

          freeList(list);
          list = NULL;
          return list;

        } else {
          strcpy(tmpParam->name, param);
          strcpy(tmpParam->value, value);

          insertBack(list, (void*)tmpParam);

          free(param);
          free(value);
          free(tmpPnV);

          tmpPnV = malloc(sizeof(char) + 2);
          j = 0;
        }
        continue;
      }

      tmpPnV[j++] = ch;
      tmpPnV = realloc(tmpPnV, sizeof(char) * j + 2);
    }

    tmpPnV[j] = '\0';

    param = getParameter(tmpPnV);
    value = getParameterValue(tmpPnV);

    tmpParam = malloc(sizeof(Parameter));
    tmpParam->name = malloc(sizeof(char) * strlen(param) + 2);
    tmpParam->value = malloc(sizeof(char) * strlen(value) + 2);

    /*If invalid parameter or value free everything */
    if(param[0] == '\0' || value[0] == '\0') {
      free(param);
      free(value);
      if(tmpPnV != NULL) {free(tmpPnV);}
      if(temp != NULL) {free(temp);}

      if(tmpParam->name != NULL) {free(tmpParam->name);}
      if(tmpParam->value != NULL) {free(tmpParam->value);}
      if(tmpParam != NULL) {free(tmpParam);}

      freeList(list);
      list = NULL;
      return list;

    } else {
      strcpy(tmpParam->name, param);
      strcpy(tmpParam->value, value);

      insertBack(list, (void*)tmpParam);

      if(param != NULL) {free(param);}
      if(value != NULL) {free(value);}
      if(tmpPnV != NULL) {free(tmpPnV);}
      if(temp != NULL) {free(temp);}
      return list;
    }
  } 
  else {
    if(temp != NULL) {free(temp);}
    return list;
  }

  if(temp != NULL) {free(temp);}
  return list;
}

/* This function retrieves the values list from the contentline given
** @param line is the contentline
** @param lineLen is the length of the contentLine */
List *getValuesList(char *line, int lineLen) {

  char ch = ' ';
  char *tempCL = NULL;
  char *tempVal = NULL;
  char *valueNode = NULL;
  int i = 0;
  int j = 0;
  
  List *list = initializeList(&valueToString, &deleteValue, &compareValues);

  tempCL = getStringRightColon(line, lineLen);
  
  // printf("Right str is %s\n", tempCL);

  if(tempCL == NULL || tempCL[0] == '\0') {
    if(tempCL != NULL) {free(tempCL);}
    if(list != NULL) {freeList(list);}
    return NULL;
  }
  else if(isSemiColon(tempCL, strlen(tempCL)+1)) {

    tempVal = malloc(sizeof(char) + 2);

    for(i =  0; i < strlen(tempCL) + 1; i++) {
      ch = tempCL[i];

      if(ch == ';') {
        tempVal[j] = '\0';

        valueNode = malloc(sizeof(char) * strlen(tempVal) + 1);
        strcpy(valueNode, tempVal);
        
        insertBack(list, (void*)valueNode);
        
        free(tempVal);
        tempVal = malloc(sizeof(char) + 2);
        j = 0;
        continue;
      }
      
      tempVal[j++] = ch;
      tempVal = realloc(tempVal, sizeof(char) * j + 2);
    }

    tempVal[j] = '\0';

    valueNode = malloc(sizeof(char) * strlen(tempVal) + 1);
    strcpy(valueNode, tempVal);
    
    insertBack(list, (void*)valueNode);
    
    if(tempVal != NULL) {free(tempVal);}
    if(tempCL != NULL) {free(tempCL);}
    return list;
  }
  else {
    valueNode = malloc(sizeof(char) * strlen(tempCL) + 1);
    strcpy(valueNode, tempCL);

    insertBack(list, (void*)valueNode);

    free(tempCL);
    return list;
  }
  
  return list;
}

/* This function retrieves the property value from the contentline given
** @param line is the contentline
** @param lineLen is the length of the contentLine */
char *getProperty(char *line, int lineLen) {

  /* Declaring variables */
  char ch = ' ';
  char *propertyBuff = malloc(sizeof(char) + 2);
  char *temp = NULL;
  int charCount = 0;
  int i = 0;
  int j = 0;
  int groupDelimIndex = 0;

  /* Check colon exists and get everything before it */
  temp = getStringLeftColon(line, &charCount, lineLen);

  // printf("TEMP IS %s\n", temp);

  if(temp == NULL) {
    return temp;
  }

  /* Check for groups and parameters */

  /*If has both groups and parameters*/
  if(isSemiColon(temp, charCount) == true && isGroupDelim(temp, charCount) == true) {
    groupDelimIndex = getGroupDelimIndex(temp, charCount);
    i = 0;
    j = groupDelimIndex + 1;
    
      //Loop through temp buffer until first semi colon to get the property name
    while(ch != ';') {
      ch = temp[j++];
      propertyBuff[i++] = ch;
      propertyBuff = realloc(propertyBuff,sizeof(char) * i + 2);
      ch = temp[j];
    }

  } 
  /*If has just parameters */
  else if(isSemiColon(temp, charCount) == true) {
    ch = ' '; //reinit ch
    i = 0;

    //Loop through temp buffer until first semi colon to get the property name
    while(ch != ';') {
      ch = temp[i];
      propertyBuff[i++] = ch;
      propertyBuff = realloc(propertyBuff,sizeof(char) * i + 2);
      ch = temp[i];
    }
  } 
  /*If has just groups */
  else if(isGroupDelim(temp, charCount) == true) {
    groupDelimIndex = getGroupDelimIndex(temp, charCount);
    ch = ' ';
    i = 0;
    j = groupDelimIndex + 1;

    while(ch != '\0') {
      ch = temp[j++];
      propertyBuff[i++] = ch;
      propertyBuff = realloc(propertyBuff,sizeof(char) * i + 2);
      ch = temp[j];
    }
  }
  /*If has neither group or parameters */
  else {
     if(propertyBuff != NULL) {free(propertyBuff);}
      propertyBuff = temp;
      return propertyBuff;
  }

  if(temp != NULL){free(temp);}
  propertyBuff[i] = '\0';
  return propertyBuff;
}

/* This function retrieves the string up until the first semicolon of a contentline 
** @param line is the contentline
** @param chCount is a pointer to the character counter for the size of the new string
** @param lineLen is the length of the contentLine going in */
char *getStringLeftColon(char *line, int *chCount, int lineLen) {

  char ch = ' ';
  char* str = malloc(sizeof(char) + 2);
  int i = 0;
  int charCount = *chCount;

  /* Check colon exists and get everything before it */
  if(isColon(line, lineLen)) {
   
    /* If the first character is a semicolon */
    if(line[0] == ':') {
      str[0] = '\0';
      charCount++;
      return str;
    }
    while(ch != ':') {
      ch = line[i];
      str[i++] = ch;
      str = realloc(str,sizeof(char) * i + 1);
      ch = line[i]; //So that it doesn't take the semi colon into the prop buff
      charCount++; //Keep track of amount of characters before first semicolon
    }
  } else {
    free(str);
    str = NULL;
    return str;
  }

  str[i] = '\0';

  *chCount = charCount;
  return str;
}

/* This function retrieves the string up until the first semicolon of a contentline 
** @param line is the contentline
** @param chCount is a pointer to the character counter for the size of the new string
** @param lineLen is the length of the contentLine going in */
char *getStringRightColon(char *line, int lineLen) {

  char *str = malloc(sizeof(char) + 2);
  // char *str = NULL;
  int i = 0;
  int j = 0;
  // int len = 0;

  if(isColon(line, lineLen)) {
    i = getIndexColon(line, lineLen) + 1;
    // len = strlen(line) - i;
    // str = malloc(sizeof(char) * len + 2);

    if(line[i] == '\0') {
      if(str != NULL) {free(str);}
      str = NULL;
      return str;
    } 

    for(i = i; i < lineLen; i++) {
      str[j++] = line[i];
      str = realloc(str, sizeof(char) * j + 2);
    }
  } else {
    if(str != NULL) {free(str);}
    str = NULL;
    return str;
  }

  str[j] = '\0';

  return str;

}

/* This function retrieves the index of the colon in a content line
** @param line is the contentline
** @param lineLen is the length of the contentline */
int getIndexColon(char *line, int lineLen) {
  int i = 0;
  int index = 0;

  for(i = 0; i < lineLen; i++) {
    if(line[i] == ':') {
      index = i;
      return index;
    }
  }

  return index;
}

/* This function retrieves the first index of the semicolon from the line given
** @param line is the line to find the semicolon */
int getIndexSemiColon(char *line) {
  
  char ch = ' ';
  int i = 0;

  while(ch != ';') {
    ch = line[i++];
  }

  return i;
}

/*Get the length of the contentline from memory
** @param vcard is the string of the vcard in memory
** @param index is the spot in the string of vcard we're at*/
int getContentLineLength(char *vcard, int *index) {
  
  bool isDone = false;
  int i = *index;
  int temp = *index;
  int len = 0;

  /* Get length of the line even if it has line folding */
  while(isDone != true) {
    while((vcard[i] != '\n')) {
      i++;
    }
    if(vcard[i+1] != ' ') {
      isDone = true;
    } else {
      i++;
    }
  }

  len = i - temp;
  return len;
}

/* Gets the index of the group deliminator from the line 
** @param line is the line to check from*/
int getGroupDelimIndex(char *line, int lineLen) {
  
  /* Declaring variables */
  int i = 0;
  int delIndex = 0;

  /* Double check delim exists */
  if(isGroupDelim(line, lineLen)) {
    for(i = 0; i < lineLen; i++) {
      if(line[i] == '.') {
        delIndex = i;
      }
    }
  }

  return delIndex;
}


/********************** REQUIRED FUNCTIONS *************************/

void deleteCard(Card* obj) {
  Card *tmpCard;

  if(obj == NULL) {
    return;
  }

  tmpCard = (Card *)obj;

  deleteProperty(tmpCard->fn);
  deleteDate(tmpCard->birthday);
  deleteDate(tmpCard->anniversary);
  freeList(tmpCard->optionalProperties);

  tmpCard->fn = NULL;
  tmpCard->birthday = NULL;
  tmpCard->anniversary = NULL;
  tmpCard->optionalProperties = NULL;
  
  if(tmpCard != NULL) {
    free(tmpCard);
    tmpCard = NULL;
  }
}

char* cardToString(const Card* obj) {
  char *str;
  Card *tmpCard;
  int len;

  if(obj == NULL) {
    return NULL;
  }

  tmpCard = (Card*)obj;

  len = strlen(tmpCard->fn->name) + 20;
  str = malloc(sizeof(char) * len + 2);

  sprintf(str, "%s\n",tmpCard->fn->name);

  return str;
}

char* errorToString(VCardErrorCode err) {

  if( err == INV_FILE) {
    char *string = "INVALID FILE";
    return string;
  } else if( err == INV_PROP) {
    char *string = "CARD HAS AN INVALID PROPERTY";
    return string;
  } else if( err == OK) {
    char *string = "CARD IS OK";
    return string;
  } else if( err == INV_CARD) {
    char *string = "INVALID CARD";
    return string;
  } else if(err == INV_DT) {
    char *string = "INVALID DATE-TIME STRUCT";
    return string;
  } else if( err == WRITE_ERROR){
    char *string = "WRITE ERROR";
    return string;
  } else {
    char *string = "OTHER ERROR, NON VCARD HAS HAPPENED";
    return string;
  }
  
  return "\0";
}

void deleteProperty(void* toBeDeleted) {
  Property *tmpProp;

  if(toBeDeleted == NULL) {
    return;
  }

  tmpProp = (Property *)toBeDeleted;
  
  if(tmpProp->name != NULL) {
    free(tmpProp->name);
    tmpProp->name = NULL;
  }

  if(tmpProp->group != NULL) {
    free(tmpProp->group);
    tmpProp->group = NULL;
  }

  if(tmpProp->parameters != NULL) {
    freeList(tmpProp->parameters);
    tmpProp->parameters = NULL;
  }

  if(tmpProp->values != NULL) {
    freeList(tmpProp->values);
    tmpProp->values = NULL;
  }

  if(tmpProp != NULL) {
    free(tmpProp);
    tmpProp = NULL;
  }
}

/* Comparing properties by their name */
int compareProperties(const void* first,const void* second) {

  Property *tmp1;
  Property *tmp2;

  if(first == NULL || second == NULL) {
    return 0;
  }

  tmp1 = (Property*)first;
  tmp2 = (Property*)second;

  return strcmp((char*)tmp1->name, (char*)tmp2->name);
}

/* Prints the property to be readable for humans */
char* propertyToString(void* prop) {

  char *str;
  Property *tmpProp;
  int len;

  if(prop == NULL) {
    return NULL;
  }

  tmpProp = (Property*)prop;

  len = strlen(tmpProp->name) + strlen(tmpProp->group);
  str = malloc(sizeof(char) * len + 100);

  sprintf(str, "Property = %s Group = %s", tmpProp->name, tmpProp->group);

  return str;
}

/* Deletes a parameter Node */
void deleteParameter(void* toBeDeleted) {
  Parameter *tmpParam;

  if(toBeDeleted == NULL) {
    return;
  }

  tmpParam = (Parameter*)toBeDeleted;

  /* Freeing memory */
  if(tmpParam->name != NULL) {
    free(tmpParam->name);
    tmpParam->name = NULL;
  }

  if(tmpParam->value != NULL) {
    free(tmpParam->value);
    tmpParam->value = NULL;
  }

  if(tmpParam != NULL) {
    free(tmpParam);
    tmpParam = NULL;
  }
}

/* Comparing parameters by their names */
int compareParameters(const void* first,const void* second) {

  Parameter *tmp1;
  Parameter *tmp2;

  if(first == NULL || second == NULL) {
    return 0;
  }

  tmp1 = (Parameter*)first;
  tmp2 = (Parameter*)second;

  return strcmp((char*)tmp1->name, (char*)tmp2->name);
}

/* Prints the parameter to be readable for humans */
char* parameterToString(void* param) {
  
  char *tempStr;
  Parameter *tmpParam;
  int len;

  if(param == NULL) {
    return NULL;
  }
  
  tmpParam = (Parameter*)param;
  
  len = strlen(tmpParam->name) + strlen(tmpParam->value) + 40;
  tempStr = malloc(sizeof(char) * len);

  // sprintf(tempStr, "Parameter name is %s parameter value is %s\n", tmpParam->name, tmpParam->value);
  sprintf(tempStr, ";%s=%s", tmpParam->name, tmpParam->value);

  return tempStr;
}

/* Delete the value node */
void deleteValue(void* toBeDeleted) {

  char *tempStr;

  if(toBeDeleted == NULL) {
    return;
  }

  tempStr = (char*)toBeDeleted;

  if(tempStr != NULL) {
    free(tempStr);
    tempStr = NULL;
  }
}

/* Compare the values */
int compareValues(const void* first,const void* second) {

  char *tempStr1;
  char *tempStr2;
  
  if(first == NULL || second == NULL) {
    return 0;
  }

  tempStr1 = (char*)first;
  tempStr2 = (char*)second;

  return strcmp(tempStr1, tempStr2);
}

/* Prints the value to be readable for humans */
char* valueToString(void* val) {

  char *tempStr = NULL;

  if(val == NULL) {
    return NULL;
  }

  tempStr = (char*)val;
  char *Str = malloc(sizeof(char) * strlen(tempStr) + 25);
  sprintf(Str, "Value: %s", tempStr);

  return Str;
}

/* Delete the DateTime struct */
void deleteDate(void* toBeDeleted) {
  DateTime* tempDate;

  if(toBeDeleted == NULL) {
    return;
  }

  tempDate = (DateTime*)toBeDeleted;

  if(tempDate->date != NULL) {
    free(tempDate->date);
    tempDate->date = NULL;
  }
  if(tempDate->time != NULL) {
    free(tempDate->time);
    tempDate->time = NULL;
  }
  if(tempDate->text != NULL) {
    free(tempDate->text);
    tempDate->text = NULL;
  }
  if(tempDate != NULL) {
    free(tempDate);
    tempDate = NULL;
  }
}

int compareDates(const void* first,const void* second) {
  return 0;
}

char* dateToString(void* date) {

  char *tmpStr;
  DateTime *tmpDateTime;
  int len;

  if(date == NULL) {
    return NULL;
  }

  tmpDateTime = (DateTime*)date;

  len = strlen(tmpDateTime->date) + strlen(tmpDateTime->time) + strlen(tmpDateTime->text) + 25;
  tmpStr = malloc(sizeof(char) * len + 5);

  sprintf(tmpStr, "Date-time is %s %s %s\n", tmpDateTime->date, tmpDateTime->time, tmpDateTime->text);

  return tmpStr;
}


/* ASSIGNMENT 2 FUNCTIONS */

/* This function writes the card object into a new file */
VCardErrorCode writeCard(const char* fileName, const Card* obj) {

  if(fileName == NULL || obj == NULL) {
    return WRITE_ERROR;
  }

  /* Declaring variables */
  Card *card = (Card *)obj;
  char *fName = (char*)fileName;
  FILE *fp = NULL;
  char *string = malloc(sizeof(char) + 2);
  void *elem;
  void *nextElem;

  fp = fopen(fName, "w+");

  /*Write the first two lines*/
  fprintf(fp, "BEGIN:VCARD\r\n");
  fprintf(fp, "VERSION:4.0\r\n");

  /* Get the fn property */
  if(card->fn->name == NULL || card->fn->name[0] == '\0') {
    free(string);
    string = NULL;
    fclose(fp);
    return WRITE_ERROR;
  }
  else {
    /*Add the group */
    string[0] = '\0';
    string = realloc(string, sizeof(char) * strlen(card->fn->group) + 2);
    strcat(string, card->fn->group);
    if(string[0] != '\0') {strcat(string, ".");}

    /* Add the fn property */
    string = realloc(string, sizeof(char) * (strlen(card->fn->group) + strlen(card->fn->name)) + 10);
    strcat(string, card->fn->name);

    /* fn having parameters */
    if(card->fn->parameters != NULL) {
      ListIterator iter = createIterator(card->fn->parameters);
      while((elem = nextElement(&iter)) != NULL) {
        Parameter *tmpParam = (Parameter*)elem;
        char* buff = malloc(sizeof(char) * (strlen(tmpParam->name) + strlen(tmpParam->value)) + 10);
        sprintf(buff, ";%s=%s", tmpParam->name, tmpParam->value);
        string = realloc(string, sizeof(char) * (strlen(string) + strlen(buff)) + 10);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
    }

    /* Place the fn values */
    if(card->fn->values != NULL) {
      /* Place the ':' before the values */
      strcat(string, ":");

      ListIterator iterFnVal = createIterator(card->fn->values);
      ListIterator tmpPtr = iterFnVal;
      while((elem = nextElement(&iterFnVal)) != NULL) {
        tmpPtr = iterFnVal;
        char *tmpFnVal = (char *)elem;
        char *buff = malloc(sizeof(char) * (strlen(tmpFnVal)) + 10);
        sprintf(buff, "%s", tmpFnVal);
        string = realloc(string, sizeof(char) * (strlen(string) + strlen(buff)) + 10);
        strcat(string, buff);
        free(buff);
        buff = NULL;
        nextElem = nextElement(&tmpPtr);
        if(nextElem != NULL) {
          strcat(string, ";");
        }
      }
    }

    /* Adding carraige return and line feed */
    strcat(string, "\r\n");
    fprintf(fp, string);
    free(string);
    string = NULL; 
  }
  
  /* BIRTHDAY PROPERTY */
  if(card->birthday != NULL) {
    DateTime *tmpBday = card->birthday;
    
    if(string != NULL){free(string);}
    string = malloc(sizeof(char) + 20);
    strcpy(string, "BDAY");

    /* IF TEXT*/
    if(tmpBday->isText == true) {
      string = realloc(string, sizeof(char) * (strlen(string) + 12 + strlen(tmpBday->text)) + 10);
      char *buff = malloc(sizeof(char) * strlen(tmpBday->text) + 25);
      sprintf(buff, ";VALUE=text:%s\r\n", tmpBday->text);
      strcat(string, buff);
      free(buff);
      buff = NULL;
    }
    /* IF UTC */
    else if(tmpBday->UTC == true) {
      string = realloc(string, sizeof(char) * (strlen(string) + strlen(tmpBday->date) + strlen(tmpBday->time)) + 20);
      char *buff = malloc(sizeof(char) * (strlen(tmpBday->date) + strlen(tmpBday->time)) + 10);

      /*If only time value */
      if(tmpBday->date[0] == '\0' && tmpBday->time[0] != '\0') {
        sprintf(buff, ":T%sZ\r\n", tmpBday->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      } 
      /*If date and time */
      else if(tmpBday->date[0] != '\0' && tmpBday->time[0] != '\0') {
        sprintf(buff, ":%sT%sZ\r\n", tmpBday->date, tmpBday->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
    }
    /* If NEITHER UTC OR TEXT */
    else if(tmpBday->isText == false && tmpBday->UTC == false) {
      string = realloc(string, sizeof(char) * (strlen(string) + strlen(tmpBday->date) + strlen(tmpBday->time)) + 20);
      char *buff = malloc(sizeof(char) * (strlen(tmpBday->date) + strlen(tmpBday->time)) + 10);

      /*If only time value */
      if(tmpBday->date[0] == '\0' && tmpBday->time[0] != '\0') {
        sprintf(buff, ":T%s\r\n", tmpBday->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
      /*If only date value */
      if(tmpBday->date[0] != '\0' && tmpBday->time[0] == '\0') {
        sprintf(buff, ":%s\r\n", tmpBday->date);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
      /*If date and time */
      else if(tmpBday->date[0] != '\0' && tmpBday->time[0] != '\0') {
        sprintf(buff, ":%sT%s\r\n", tmpBday->date, tmpBday->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
    }
    /*Print to file */
    fprintf(fp, string);
    free(string);
    string = NULL;
  }

  /* ANNIVERSARY PROPERTY */
  if(card->anniversary != NULL) {
    DateTime *tmpAnn = card->anniversary;
    
    if(string != NULL){free(string);}
    string = malloc(sizeof(char) + 20);
    strcpy(string, "ANNIVERSARY");

    /* IF TEXT*/
    if(tmpAnn->isText == true) {
      string = realloc(string, sizeof(char) * (strlen(string) + 12 + strlen(tmpAnn->text)) + 10);
      char *buff = malloc(sizeof(char) * strlen(tmpAnn->text) + 25);
      sprintf(buff, ";VALUE=text:%s\r\n", tmpAnn->text);
      strcat(string, buff);
      free(buff);
      buff = NULL;
    }
    /* IF UTC */
    else if(tmpAnn->UTC == true) {
      string = realloc(string, sizeof(char) * (strlen(string) + strlen(tmpAnn->date) + strlen(tmpAnn->time)) + 20);
      char *buff = malloc(sizeof(char) * (strlen(tmpAnn->date) + strlen(tmpAnn->time)) + 10);

      /*If only time value */
      if(tmpAnn->date[0] == '\0' && tmpAnn->time[0] != '\0') {
        sprintf(buff, ":T%sZ\r\n", tmpAnn->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      } 
      /*If date and time */
      else if(tmpAnn->date[0] != '\0' && tmpAnn->time[0] != '\0') {
        sprintf(buff, ":%sT%sZ\r\n", tmpAnn->date, tmpAnn->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
    }
    /* If NEITHER UTC OR TEXT */
    else if(tmpAnn->isText == false && tmpAnn->UTC == false) {
      string = realloc(string, sizeof(char) * (strlen(string) + strlen(tmpAnn->date) + strlen(tmpAnn->time)) + 20);
      char *buff = malloc(sizeof(char) * (strlen(tmpAnn->date) + strlen(tmpAnn->time)) + 10);

      /*If only time value */
      if(tmpAnn->date[0] == '\0' && tmpAnn->time[0] != '\0') {
        sprintf(buff, ":T%s\r\n", tmpAnn->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
      /*If only date value */
      if(tmpAnn->date[0] != '\0' && tmpAnn->time[0] == '\0') {
        sprintf(buff, ":%s\r\n", tmpAnn->date);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
      /*If date and time */
      else if(tmpAnn->date[0] != '\0' && tmpAnn->time[0] != '\0') {
        sprintf(buff, ":%sT%s\r\n", tmpAnn->date, tmpAnn->time);
        strcat(string, buff);
        free(buff);
        buff = NULL;
      }
    }
    /*Print to file */
    fprintf(fp, string);
    if(string != NULL) {
      free(string);
      string = NULL;
    }
  }

  /* Optional Properties */
  if(card->optionalProperties) {
    ListIterator iterOpProp = createIterator(card->optionalProperties);

    while((elem = nextElement(&iterOpProp)) != NULL) {
      Property *tmpProp = (Property*)elem;
      string = realloc(string, sizeof(char) * sizeof(tmpProp) + strlen(tmpProp->name) + strlen(tmpProp->group) + 20);

      /* Add Group */
      string[0] = '\0';
      strcat(string, tmpProp->group);
      if(string[0] != '\0') {strcat(string, ".");}

      /* Add name */
      strcat(string, tmpProp->name);

      /* Get the parameters */
      if(tmpProp->parameters != NULL) {
        ListIterator paramIter = createIterator(tmpProp->parameters);
        void *paramElem;
        while((paramElem = nextElement(&paramIter)) != NULL) {
          Parameter *tParam = (Parameter*)paramElem;
          char *buff = malloc(sizeof(char) * (strlen(tParam->name) + strlen(tParam->value)) + 10);
          sprintf(buff, ";%s=%s", tParam->name, tParam->value);
          string = realloc(string, sizeof(char) * (strlen(string) + strlen(buff)) + 10);
          strcat(string, buff);
          free(buff);
          buff = NULL;
        }
      }

      /* Get the values */
      if(tmpProp->values != NULL) {
        /* Place the : before the values */
        strcat(string, ":");

        void *valElem;
        ListIterator iterVal = createIterator(tmpProp->values);
        ListIterator tempPtr = iterVal;
        while((valElem = nextElement(&iterVal)) != NULL) {
          tempPtr = iterVal;
          char *tmpVal = (char *)valElem;
          char *buff = malloc(sizeof(char) * (strlen(tmpVal)) + 10);
          sprintf(buff, "%s", tmpVal);
          string = realloc(string, sizeof(char) * (strlen(string) + strlen(buff)) + 10);
          strcat(string, buff);
          free(buff);
          buff = NULL;
          nextElem = nextElement(&tempPtr);
          if(nextElem != NULL) {
            strcat(string, ";");
          }
        }
      }

      /* Adding carraige return and line feed */
      strcat(string, "\r\n");
      fprintf(fp, string);
      free(string);
      string = NULL; 
    }
  }

  /* Write the last line */
  fprintf(fp, "END:VCARD\r\n");

  fclose(fp);
  if(string != NULL) {
    free(string);
    string = NULL;
  }

  return OK;

}

/* This function validates the card struct created */
VCardErrorCode validateCard(const Card* obj) {
  
  if(obj == NULL) {
    return INV_CARD;
  }

  /* Declaring variables */
  Card *tmpCard = (Card *)obj;
  int nC = 0, kC = 0, gC = 0, pC = 0, rC = 0, uC = 0;
  void *elem;

  if(tmpCard->fn == NULL || tmpCard->optionalProperties == NULL) {
    return INV_CARD;
  }

  if(tmpCard->fn->name == NULL || tmpCard->fn->group == NULL || tmpCard->fn->parameters == NULL || tmpCard->fn->values == NULL ||
     tmpCard->fn->name[0] == '\0') {
    return INV_CARD;
  }

  /* Make sure parameter name and value not empty string for fn */
  if(tmpCard->fn->parameters != NULL) {
    ListIterator iter = createIterator(tmpCard->fn->parameters);
    while((elem = nextElement(&iter)) != NULL) {
      Parameter *tmpFnParam = (Parameter *)elem;
      if(tmpFnParam->name[0] == '\0' || tmpFnParam->value[0] == '\0') {
        return INV_PROP;
      }
    }
  }  

  /* Check values of FN */
  if(tmpCard->fn->values != NULL) {
    /* Cardinality of values list is > 1 */
    if(getLength(tmpCard->fn->values) > 1) {
      return INV_PROP;
    }

    /* Check for empty string and multi-value text */
    ListIterator iterFnVal = createIterator(tmpCard->fn->values);
    while((elem = nextElement(&iterFnVal)) != NULL) {
      char *tmpFnVal = (char*)elem;
      if(tmpFnVal[0] == '\0') {
        return INV_PROP;
      }
      if(isMultiValText(tmpFnVal)) {
        return INV_PROP;
      }
    }
  }

  /* Iterate through the list of optional properties and make sure they're okay */
  ListIterator iterOpProp = createIterator(tmpCard->optionalProperties);
  while((elem = nextElement(&iterOpProp)) != NULL) {
    Property *tmpProp = (Property *)elem;

    if(validateProp(tmpProp, &nC, &kC, &gC, &pC, &rC, &uC) == false) {
      return INV_PROP;
    }

    if(strcmpCI(tmpProp->name, "VERSION") == 0) {
      return INV_CARD;
    }

    if(strcmpCI(tmpProp->name, "BDAY") == 0 || strcmpCI(tmpProp->name, "ANNIVERSARY") == 0) {
      return INV_DT;
    }
  }
  
  /* Validate the BIRTHDAY Property */
  if(tmpCard->birthday != NULL) {
    if(isValidDateTime(tmpCard->birthday) == false) {
      return INV_DT;
    }
  }

  /* Validate the ANNIVERSARY Property */
  if(tmpCard->anniversary != NULL) {
    if(isValidDateTime(tmpCard->anniversary) == false) {
      return INV_DT;
    }
  }

  return OK;
}

/* This function takes in a property and checks if it is valid or not 
   @param property is the Property object
   @param nC is N counter
   @param kC is KIND counter
   @param gC is GENDER counter
   @param pC is PRODID counter
   @param rC is REV counter
   @param uC is UID counter*/
bool validateProp(Property *property, int *nC, int *kC, int *gC, int *pC, int *rC, int *uC) {
  
  /* Declaring variables */
  void *elem;

  /* Check for NULL */
  if(property->name == NULL || property->name[0] == '\0' || property->group == NULL || property->parameters == NULL || 
     property->values == NULL) {
    return false;
  }

  /* Check valid property name */
  if(isValidProperty(property->name) == false) {
    return false;
  }

  if(strcmpCI(property->name, "BEGIN") == 0 || strcmpCI(property->name, "END") == 0) {
    return false;
  }

  /* Make sure parameter name and value not empty string*/
  if(property->parameters != NULL) {
    ListIterator iter = createIterator(property->parameters);
    while((elem = nextElement(&iter)) != NULL) {
      Parameter *tmpFnParam = (Parameter *)elem;
      if(tmpFnParam->name[0] == '\0' || tmpFnParam->value[0] == '\0') {
        return false;
      }
    }
  }

  /* If nothing in the values list */
  if(getLength(property->values) == 0) {
    return false;
  }

  /* FN PROPERTY */
  if(strcmpCI(property->name, "FN") == 0) {
    if(getLength(property->values) > 1) {
      return false;
    }

    if(isMultiVal(property) == true) {
      return false;
    }
  }

  /* N PROPERTY */
  if(strcmpCI(property->name, "N") == 0) {
    *nC = *nC + 1;
    if(*nC > 1) {
      return false;
    }
    
    if(getLength(property->values) > 5) {
      return false;
    }
  }

  /* KIND PROPERTY */
  if(strcmpCI(property->name, "KIND") == 0) {
    *kC = *kC + 1;
    if(*kC > 1) {
      return false;
    }

    if( (getLength(property->values) > 1) || (isMultiVal(property) == true)) {
      return false;
    }
  }

  /* XML PROPERTY */
  if(strcmpCI(property->name, "XML") == 0) {
    if( (getLength(property->values) > 1) || (isMultiVal(property) == true)) {
      return false;
    }
  }

  /* NICKNAME PROPERTY*/
  if(strcmpCI(property->name, "NICKNAME") == 0) {
    if(getLength(property->values) > 1) {
      return false;
    }
  }

  /* GENDER PROPERTY */
  if(strcmpCI(property->name, "GENDER") == 0) {
    *gC = *gC + 1;
    if(*gC > 1) {
      return false;
    }

    if(getLength(property->values) > 2 || isMultiVal(property) == true) {
      return false;
    }

    void *element = getFromFront(property->values);
    char *buff = (char *)element;
    if(isValidSex(buff) == false) {
      return false;
    }
  }

  /* ADDRESS PROPERTY */
  if(strcmpCI(property->name, "ADR") == 0) {
    if(getLength(property->values) != 7) {
      return false;
    }
  }

  /* EMAIL PROPERTY */
  if(strcmpCI(property->name, "EMAIL") == 0) {
    if(getLength(property->values) > 1 || isMultiVal(property) == true) {
      return false;
    }
  }

  /* LANGUAGE PROPERTY */
  if(strcmpCI(property->name, "LANG") == 0) {
    if(getLength(property->values) > 1) {
      return false;
    }
  }

  /* TITLE PROPERTY */
  if(strcmpCI(property->name, "TITLE") == 0) {
     if(getLength(property->values) > 1 || isMultiVal(property) == true) {
      return false;
    }
  }

  /* ROLE PROPERTY */
  if(strcmpCI(property->name, "ROLE") == 0) {
     if(getLength(property->values) > 1 || isMultiVal(property) == true) {
      return false;
    }
  }

  /* CATEGORIES PROPERTY */
  if(strcmpCI(property->name, "ROLE") == 0) {
    if(getLength(property->values) > 1) {
      return false;
    }
  }

  /* NOTE PROPERTY */
  if(strcmpCI(property->name, "NOTE") == 0) {
    if(getLength(property->values) > 1) {
      return false;
    }
  }

  /* PRODID PROPERTY */
  if(strcmpCI(property->name, "PRODID") == 0) {
    *pC = *pC + 1;
    if(*pC > 1) {
      return false;
    }

    if(getLength(property->values) > 1) {
      return false;
    }
  }

  /* REV PROPERTY */
  if(strcmpCI(property->name, "REV") == 0) {
    *rC = *rC + 1;
    if(*rC > 1) {
      return false;
    }
    
    if(getLength(property->values) > 1) {
      return false;
    }
  }

  /* UID PROPERTY */
  if(strcmpCI(property->name, "UID") == 0) {
    *uC = *uC + 1;
    if(*uC > 1) {
      return false;
    }
  }
  return true;
}

/* Checks the Sex of the Gender */
bool isValidSex(char *text) {
  
  if(strcmp(text, "\0") == 0 || strcmp(text, "M") == 0 || strcmp(text, "F") == 0 || 
     strcmp(text, "O") == 0 || strcmp(text, "N") == 0 || strcmp(text, "U") == 0) {
       return true;
     }

  return false;
}

/* Check if property has a multivalue text value */
bool isMultiVal(Property *prop) {

  void *elem;

  if(prop->values == NULL) {
    return false;
  }

   /* Check for empty string and multi-value text */
  ListIterator iter = createIterator(prop->values);
  while((elem = nextElement(&iter)) != NULL) {
    char *tmpVal = (char*)elem;
    if(isMultiValText(tmpVal)) {
      return true;
    }
  }

  return false;
}

/* Checks if theres more than one value in a text value in a string*/
bool isMultiValText(char *text) {
    
  if(text == NULL) {
    return false;
  }

  char ch = text[0];
  int i = 0;

  while(ch != '\0') {
    if(ch == ',') {
      return true;
    }
    ch = text[i++];
  }
  
  return false;
}

/* Checks if datetime struct is valid */
bool isValidDateTime(DateTime *dt) {
  
  if(dt == NULL) {
    return true;
  }

  char ch = ' ';
  int i = 0;

  /* IF UTC AND TEXT TRUE */
  if(dt->isText == true && dt->UTC == true) {
    return false;
  }

  if(dt->isText == true) {
    if(dt->date[0] != '\0' || dt->time[0] != '\0') {
      return false;
    }
    if(dt->text[0] == '\0') {
      return false;
    }
  } 
  else if(dt->isText == false) {
    if(dt->text[0] != '\0') {
      return false;
    }

    if(dt->time[0] == '\0' && dt->date[0] == '\0') {
      return false;
    }

    /* Check if date has any letters */
    ch = dt->date[0];
    while(ch != '\0') {
      ch = dt->date[++i];
      if(isalpha(ch) != 0) {
        return false;
      }
    }

    /* Check length of date */
    if(strlen(dt->date) > 8) {
      return false;
    }

    /* Check if time has any letters */
    i = 0;
    ch = dt->time[0];
    while(ch != '\0') {
      ch = dt->time[++i];
      if(isalpha(ch) != 0) {
        return false;
      }
    }
    i = 0;

    /* Check length of date */
    if(strlen(dt->time) > 11) {
      return false;
    }
  }

  return true;
}

/* This function retrieves the index of the quotes */
void getIndexQuote(char *string, int *currIndex, int *indexOne, int *indexTwo) {

  bool fin = false;
  bool qOne = false;
  bool qTwo = false;
  int i = *currIndex;
  char ch = string[i];

  while(fin != true) {
    if(ch == '"') {
      if(qOne == false && qTwo == false) {
        qOne = true;
        *indexOne = i;
      }
      else if(qOne == true && qTwo == false) {
        qTwo = true;
        *indexTwo = i;
        *currIndex = i + 1;
      }
    }
    if(qOne == true && qTwo == true) {
      fin = true;
    }
    ch = string[++i];
  }
}

/* This function returns a json string for a char list */
char* strListToJSON(const List* strList) {
  
  if(strList == NULL) {
    return NULL;
  }

  /* Declare variables */
  List *tmpList = (List *)strList;
  char *string = NULL;
  char *tmpStr = NULL;
  int c = 0;
  void *elem;

  /* Empty List */
  if(getLength(tmpList) == 0) {
    string = malloc(sizeof(char) + 5);
    strcpy(string, "[]");
    return string;
  }

  string = malloc(sizeof(char));
  tmpStr = malloc(sizeof(char));
  tmpStr[0] = '\0';

  /* Loop through list and place values in a tmp string with quotes */
  ListIterator iter = createIterator(tmpList);
  while((elem = nextElement(&iter)) != NULL) {
    char *buff = (char *)elem;
    tmpStr = realloc(tmpStr, sizeof(char) * (strlen(tmpStr) + strlen(buff) + 10));
    c++;
    if(c == 1) {
      strcat(tmpStr,"\"");
    }
    if(c > 1) {
      strcat(tmpStr,",\"");
    }
    strcat(tmpStr,buff);
    strcat(tmpStr,"\"");
  }

  string = realloc(string, sizeof(char) * (strlen(tmpStr)) + 10);
  sprintf(string, "[%s]", tmpStr);

  if(tmpStr != NULL) {
    free(tmpStr);
    tmpStr = NULL;
  }

  return string;
}

/* This function returns a list from the json char string */
List* JSONtoStrList(const char* str) {
  if(str == NULL) {
    return NULL;
  }

  /* Declaring variables */
  char *buff = NULL;
  // char *tmpStr = NULL;
  int index = 0;
  int iQOne;
  int iQTwo;
  int i = 0;
  int j = 0;
  int lenStr = strlen(str);
  List *newList = initializeList(&valueToString, &deleteValue, &compareValues);

  /* Take off the [] from strList */
  buff = malloc(sizeof(char) * (lenStr) + 20 );
  for(i = 1; i < lenStr - 1; i++) {
    buff[j++] = str[i];
  }
  buff[j] = '\0';

  /*Reset i and j */
  i = 0;
  j = 0;

  while(buff[index] != '\0') {
    getIndexQuote(buff, &index, &iQOne, &iQTwo);
    char *valueNode = malloc(sizeof(char*) * (iQTwo - iQOne) + 20);

    /* Copy the value into a tmpStr node */
    j = 0;
    for(i = iQOne + 1; i < iQTwo; i++) {
      valueNode[j++] = buff[i];
    }
    valueNode[j] = '\0';

    // printf("Node is %s\n", valueNode);

    insertBack(newList, (void*)valueNode);
  }
  // printf("BUFFER: %s\n", buff);

  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }

  return newList;
}

/* Chnage the property to the JSON string */
char* propToJSON(const Property* prop) {

  if(prop == NULL) {
    return NULL;
  }

  /*Declare variables*/
  char *valStr = strListToJSON(prop->values);
  char *propJSON = malloc(sizeof(char) * (strlen(prop->group) + strlen(prop->name) + strlen(valStr)) + 100 );
  propJSON[0] = '\0';

  sprintf(propJSON, "{\"group\":\"%s\",\"name\":\"%s\",\"values\":%s}", prop->group, prop->name, valStr);

  if(valStr != NULL) {
    free(valStr);
    valStr = NULL;
  }

  return propJSON;
}

/* Returns a property struct from the JSON string */
Property* JSONtoProp(const char* str) {

  if(str == NULL) {
    return NULL;
  }

  /* Declare variables */
  char ch = ' ';
  char *buff = NULL;
  char *tmpStr = NULL;
  int i = 0;
  int j = 0;
  int qCount = 0;
  int qThree, qFour, qSeven, qEight; //Quotes index
  int bOne, bTwo; //Array bracket index
  int lenStr = strlen(str);
  Property *prop = malloc(sizeof(Property));

  /* Take off the {} from strList */
  buff = malloc(sizeof(char) * (lenStr) + 20 );
  for(i = 1; i < lenStr - 1; i++) {
    buff[j++] = str[i];
  }
  buff[j] = '\0';

  /* Set variables */
  i = 0;
  j = 0;
  ch = buff[0];

  /* Get the index of the quotations*/
  while(ch != '\0') {
    if(ch == '"') {
      qCount++;
    }
    if(qCount == 3) {qThree = i;}
    if(qCount == 4) {qFour = i;}
    if(qCount == 7) {qSeven = i;}
    if(qCount == 8) {qEight = i;}
    ch = buff[i++];
  }

  ch = buff[0];
  i = 0;
  /* Get the index of the List */
  while(ch != '\0') {
    if(ch == '[') {
      bOne = i;
    }
    if(ch == ']') {
      bTwo = i;
    }
    ch = buff[++i];
  }

  /* Get the group */
  i = 0;
  j = 0;
  ch = buff[0];
  tmpStr = malloc(sizeof(char) * (qFour - qThree) + 3);
  for(i = qThree + 1; i < qFour; i++) {
    tmpStr[j++] = buff[i];
  }
  tmpStr[j] = '\0';

  prop->group = malloc(sizeof(char) * strlen(tmpStr) + 3);
  strcpy(prop->group, tmpStr);
  free(tmpStr);

  /* Get the name */
  i = 0;
  j = 0;
  ch = buff[0];
  tmpStr = malloc(sizeof(char) * (qEight - qThree) + 3);
  for(i = qSeven + 1; i < qEight; i++) {
    tmpStr[j++] = buff[i];
  }
  tmpStr[j] = '\0';

  prop->name = malloc(sizeof(char) * (strlen(tmpStr)) + 3);
  strcpy(prop->name, tmpStr);
  free(tmpStr);

  /* Initialize parameters list */
  prop->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);

  /* Get the list of values */
  i = 0;
  j = 0;
  ch = buff[0];
  tmpStr = malloc(sizeof(char) * (bTwo - bOne) + 3);
  for(i = bOne; i <= bTwo; i++) {
    tmpStr[j++] = buff[i];
  }
  tmpStr[j] = '\0';

  // printf("STRING = %s", tmpStr);
  prop->values = JSONtoStrList(tmpStr);

  if(tmpStr != NULL) {
    free(tmpStr);
    tmpStr = NULL;
  }

  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }

  return prop;
}

/* This function returns a JSON string made from the datetime struct passed into it */
char* dtToJSON(const DateTime* prop) {

  if(prop == NULL) {
    return NULL;
  }

  /* Delcaring variables */
  char *textStr = NULL;
  char *dateStr = NULL;
  char *timeStr = NULL;
  char *utcBool = NULL;
  char *textBool = NULL;
  char *strJSON = NULL;
  
  if(prop->isText == true && prop->UTC == false) {
    /* Malloc the variables */
    dateStr = malloc(sizeof(char) * 3);
    timeStr = malloc(sizeof(char) * 3);
    textStr = malloc(sizeof(char) * strlen(prop->text) + 3);
    utcBool = malloc(sizeof(char) * 7);
    textBool = malloc(sizeof(char) * 7);

    /* Set the variables */
    dateStr[0] = '\0';
    timeStr[0] = '\0';
    strcpy(textStr, prop->text);
    strcpy(utcBool, "false");
    strcpy(textBool, "true");
  }
  else if(prop->isText == false && prop->UTC == true) {
    /* Malloc the variables */
    dateStr = malloc(sizeof(char) * strlen(prop->date) + 3);
    timeStr = malloc(sizeof(char) * strlen(prop->time) + 3);
    textStr = malloc(sizeof(char) * 3);
    utcBool = malloc(sizeof(char) * 7);
    textBool = malloc(sizeof(char) * 7);

    textStr[0] = '\0';
    strcpy(dateStr, prop->date);
    strcpy(timeStr, prop->time);
    strcpy(utcBool, "true");
    strcpy(textBool, "false");
  }
  else if(prop->isText == false && prop->UTC == false) {
    /* Malloc the variables */
    dateStr = malloc(sizeof(char) * strlen(prop->date) + 3);
    timeStr = malloc(sizeof(char) * strlen(prop->time) + 3);
    textStr = malloc(sizeof(char) * 3);
    utcBool = malloc(sizeof(char) * 7);
    textBool = malloc(sizeof(char) * 7);

    textStr[0] = '\0';
    strcpy(dateStr, prop->date);
    strcpy(timeStr, prop->time);
    strcpy(utcBool, "false");
    strcpy(textBool, "false");
  }

  strJSON = malloc(sizeof(char) * (strlen(textStr) + strlen(dateStr) + strlen(timeStr) + strlen(utcBool) + strlen(textBool)) + 100);

  sprintf(strJSON, "{\"isText\":%s,\"date\":\"%s\",\"time\":\"%s\",\"text\":\"%s\",\"isUTC\":%s}", textBool, dateStr, timeStr, textStr, utcBool);

  free(textStr);
  free(dateStr);
  free(timeStr);
  free(utcBool);
  free(textBool);

  return strJSON;
}

/* Returns a datetime struct from JSON string */
DateTime* JSONtoDT(const char* str) {

  if(str == NULL) {
    return NULL;
  }

  /* Declaring variables */
  DateTime *dt = malloc(sizeof(DateTime));
  bool isFive = false;
  bool isSix = false;
  bool isNine = false;
  bool isTen = false;
  bool isThirteen = false;
  bool isFourteen = false;
  bool isSemOne = false;
  bool isSemTwo = false;
  char ch = ' ';
  char *buff = NULL;
  char *tmpStr = NULL;
  int i = 0;
  int j = 0;
  int qFive, qSix, qNine, qTen, qThirteen, qFourteen, qCount;
  int semOne, semFive, semCount;

  qCount = 0;
  /* Get the index of the quotations*/
  while(ch != '\0') {
    if(ch == '"') {
      qCount++;
    }
    if(qCount == 5 && isFive == false) {qFive = i; isFive = true;}
    if(qCount == 6 && isSix == false) {qSix = i; isSix = true;}
    if(qCount == 9 && isNine == false) {qNine = i; isNine = true;}
    if(qCount == 10 && isTen == false) {qTen = i; isTen = true;}
    if(qCount == 13 && isThirteen == false) {qThirteen = i; isThirteen = true;}
    if(qCount == 14 && isFourteen == false) {qFourteen = i; isFourteen = true;}
    ch = str[++i];
  }

  /* Get the semi colon destination for the truth values */
  semCount = 0;
  i = 0;
  ch = str[0];
  while(ch != '\0') {
    if(ch == ':') {
      semCount++;
    }
    if(semCount == 1 && isSemOne == false) {semOne = i; isSemOne = true;}
    if(semCount == 5 && isSemTwo == false) {semFive = i; isSemTwo = true;}
    ch = str[++i];
  }

  if(str[semOne + 1] == 't' && str[semFive + 1] == 'f') {
    
    dt->date = malloc(sizeof(char) * 3);
    dt->time = malloc(sizeof(char) * 3);
    
    dt->isText = true;
    dt->UTC = false;
    dt->date[0] = '\0';
    dt->time[0] = '\0';

    /* Get text */
    j = 0;
    tmpStr = malloc(sizeof(char) * (qFourteen - qThirteen) + 10);
    for(i = qThirteen + 1; i < qFourteen; i++) {
      tmpStr[j++] = str[i];
    }
    tmpStr[j] = '\0';

    dt->text = malloc(sizeof(char) * strlen(tmpStr) + 10);
    strcpy(dt->text, tmpStr);
    free(tmpStr);
    tmpStr = NULL;
  }
  else {
    
    /* Set UTC and isText booleans */
    if(str[semOne + 1] == 'f' && str[semFive + 1] == 't') {
      dt->UTC = true;
      dt->isText = false;
    } else if(str[semOne + 1] == 'f' && str[semFive + 1] == 'f') {
      dt->UTC = false;
      dt->isText = false;
    }

    dt->text = malloc(sizeof(char) * 3);
    dt->text[0] = '\0';

    /* Get the date */
    j = 0;
    tmpStr = malloc(sizeof(char) * (qSix - qFive) + 10);
    for(i = qFive + 1; i < qSix; i++) {
      tmpStr[j++] = str[i];
    }
    tmpStr[j] = '\0';

    dt->date = malloc(sizeof(char) * (strlen(tmpStr)) + 10);
    strcpy(dt->date, tmpStr);
    free(tmpStr);
    tmpStr = NULL;

    /* Get the time */
    j = 0;
    tmpStr = malloc(sizeof(char) * (qTen - qNine) + 10);
    for(i = qNine + 1; i < qTen; i++) {
      tmpStr[j++] = str[i];
    }
    tmpStr[j] = '\0';
    
    dt->time = malloc(sizeof(char) * (strlen(tmpStr)) + 10);
    strcpy(dt->time, tmpStr);
    free(tmpStr);
    tmpStr = NULL;
  }

  if(tmpStr != NULL) {
    free(tmpStr);
    tmpStr = NULL;
  }
  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }
  return dt;
}

/* Returns a Card object from the JSON string */
Card* JSONtoCard(const char* str) {

  if(str == NULL) {
    return NULL;
  }

  /* Declare variables */
  Card *card = malloc(sizeof(Card));
  char ch = ' ';
  char *buff = NULL;
  char *tempStr = NULL;
  int lenStr = strlen(str);
  int index = 0;
  int i = 0;
  int j = 0;
  int qCount = 0;

  /* Take off the {} from strList */
  buff = malloc(sizeof(char) * (lenStr) + 20 );
  for(i = 1; i < lenStr - 1; i++) {
    buff[j++] = str[i];
  }
  buff[j] = '\0';  

  ch = buff[0];

  /* Find location of 3rd quotaion mark */
  i = 0;
  while(ch != '\0') {
    if(ch == '"') {
      qCount++;
    }
    if(qCount == 3) {
      index = i;
    }
    ch = buff[i++];
  }

  tempStr = malloc(sizeof(char) * ((strlen(buff) - index)) + 3);
  j = 0;
  for(i = index + 1; i < strlen(buff) - 1; i++) {
    tempStr[j++] = buff[i];
  }
  tempStr[j] = '\0';

  /* Fill the card struct witht the data */
  card->fn = malloc(sizeof(Property));
  card->fn->name = malloc(sizeof(char) * 3 );
  strcpy(card->fn->name, "FN");
  card->fn->group = malloc(sizeof(char) * 3);
  card->fn->group[0] = '\0';
  card->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
  card->birthday = NULL;
  card->anniversary = NULL;
  card->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);


  card->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
  insertBack(card->fn->values, (void*)tempStr);
  
  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }

  return card;
}

/* Adds a property to the end of the optional property list*/
void addProperty(Card* card, const Property* toBeAdded) {
  if(toBeAdded == NULL || card == NULL) {
    return;
  }
  insertBack(card->optionalProperties, (void*)toBeAdded);
}

/******* A3 Functions ********/

/* This checks if a vcard is valid and returns a string */
char* validateVCard(char *fileName) {

  /* Declaring variables */
  Card *newCard = NULL;
  char *str = NULL;

  str = malloc(sizeof(char) * 15);

  if(fileName == NULL) {
    strcpy(str, "false");
    return str;
  }

  /* Create and validate the card */
  if(createCard(fileName, &newCard) == OK) {
    if(validateCard(newCard) == OK) {
      deleteCard(newCard);
      strcpy(str, "true");
      return str;
    }
  }

  if(newCard != NULL) {
    deleteCard(newCard);
  }

  strcpy(str,"false");

  return str;
}

int getNumOptionalProp(char *fileName) {
  
  /* Declaring variables */
  Card *newCard = NULL;
  int num = 0;

  if(createCard(fileName, &newCard) == OK) {
    if(validateCard(newCard) == OK) {
      num = getLength(newCard->optionalProperties);
    }
  }

  if(newCard != NULL) {
    deleteCard(newCard);
    newCard = NULL;
  }

  return num;
}

bool createNewVCard(char *fileName, char *FN) {
  /* Declare Variables */
  bool valid = false;
  char *file = fileName;
  Card *newCard = NULL;
  int i = 0;
  int j = 0;
  int index = 0;
  char *extension = NULL;

  for(i = 0; i < strlen(file); i++) {
    if(file[i] == '.') {
      index = i;
    }
  }

  extension = malloc(sizeof(char) + (strlen(file)));

  for(i = index + 1; i < strlen(file); i++) {
    extension[j] = file[i];
    j++;
  }
  extension[j] = '\0';

  if(strcmp(extension, "vcf") == 0) {
    valid = true;
  } else if(strcmp(extension, "vcard") == 0) {
    valid = true;
  }

  if(valid == false) {
    free(extension);
    return false;
  }

  char *val = malloc(strlen(FN) + 10);
  strcpy(val, FN);

  /*Initialize the new card */
  newCard = malloc(sizeof(Card));
  newCard->fn = malloc(sizeof(Property));
  newCard->fn->name = malloc(sizeof(char) + 5);
  newCard->fn->group = malloc(sizeof(char) + 5);
  newCard->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
  newCard->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
  newCard->birthday = NULL;
  newCard->anniversary = NULL;
  newCard->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);
  insertBack(newCard->fn->values, (void*)val);
  strcpy(newCard->fn->name, "FN");
  newCard->fn->group[0] = '\0';

  /*Make sure the card is valid*/
  if(validateCard(newCard) == OK) {
    if(writeCard(fileName, newCard) == OK) {
      deleteCard(newCard);
      return true;
    }
    else {
      deleteCard(newCard);
      return false;
    }
  } else {
    deleteCard(newCard);
  }

  return false;
}

/* This functions assumes the file has already been validated 
    @fileName is the entire file path
    @name is the fileName */
char *getCardJSON(char *fileName, char *name) {

  /* Declaring variables */
  Card *newCard = NULL;
  char *fnJson = NULL;
  char *bdayJson = NULL;
  char *annJson = NULL;
  char *strJson = malloc(sizeof(char) + 3);
  char *opPropJson = NULL;
  int numProp = 0;

  if(createCard(fileName, &newCard) == OK) {
    if(validateCard(newCard) == OK) {
      fnJson = propertyToJSON(newCard->fn);
      bdayJson = dtToJSON(newCard->birthday);
      annJson = dtToJSON(newCard->anniversary);
      opPropJson = getOptionalPropJSON(newCard->optionalProperties);
    }
  }

  strJson[0] = '\0';
  numProp = getNumOptionalProp(fileName);

  if(bdayJson != NULL && annJson != NULL) {
    strJson = realloc(strJson, sizeof(char) * (strlen(fnJson) + strlen(bdayJson) + strlen(annJson) + strlen(opPropJson)) + 500);
    sprintf(strJson, "{\"fileName\":\"%s\",\"fn\":%s,\"birthday\":%s,\"anniversary\":%s,\"numProp\":%d,\"optionalProperties\":%s}", 
            name, fnJson, bdayJson, annJson, numProp, opPropJson);
  }
  else if(bdayJson == NULL && annJson != NULL) {
    strJson = realloc(strJson, sizeof(char) * (strlen(fnJson) + strlen(annJson) + strlen(opPropJson)) + 500);
    sprintf(strJson, "{\"fileName\":\"%s\",\"fn\":%s,\"anniversary\":%s,\"numProp\":%d,\"optionalProperties\":%s}", name, fnJson, annJson, numProp, opPropJson);
  }
  else if(bdayJson != NULL && annJson == NULL) {
    strJson = realloc(strJson, sizeof(char) * (strlen(fnJson) + strlen(bdayJson) + strlen(opPropJson)) + 500);
    sprintf(strJson, "{\"fileName\":\"%s\",\"fn\":%s,\"birthday\":%s,\"numProp\":%d,\"optionalProperties\":%s}",name, fnJson, bdayJson, numProp, opPropJson);
  }
  else if(bdayJson == NULL && annJson == NULL) {
    strJson = realloc(strJson, sizeof(char) * (strlen(fnJson) + strlen(opPropJson)) + 500);
    sprintf(strJson, "{\"fileName\":\"%s\",\"fn\":%s,\"numProp\":%d,\"optionalProperties\":%s}", name, fnJson, numProp, opPropJson);
  }

  return strJson;
}

char *getOptionalPropJSON(List *opList) {

  if(opList == NULL) {
    return NULL;
  }
  
  /* Declaring variables */
  List *tmpList = opList;
  char *JSONStr = NULL;

  /* Empty List */
  if(getLength(tmpList) == 0) {
    JSONStr = malloc(sizeof(char) + 10);
    strcpy(JSONStr, "[]");
  }

  JSONStr = malloc(sizeof(char) + 15);
  JSONStr[0] = '[';
  JSONStr[1] = '\0';

  void *elem;

  /* Iterate through the optional properties list and get the JSON string of the property*/
  ListIterator iter = createIterator(tmpList);
  while((elem = nextElement(&iter)) != NULL) {
    Property *tmpProp = (Property*)elem;
    char* buff = propertyToJSON(tmpProp);
    JSONStr = realloc(JSONStr, sizeof(char) * (strlen(buff) + strlen(JSONStr) + 50));
    strcat(JSONStr, buff);
    strcat(JSONStr, ",");
  }

  int i = strlen(JSONStr);

  if(strcmp(JSONStr, "[") == 0) {
    JSONStr = realloc(JSONStr, sizeof(char) + 10);
    strcpy(JSONStr, "[]");
  } else {
    JSONStr[i-1] = ']';
  }
  
  return JSONStr;
}

/* Chnage the property to the JSON string WITH PARAMETERS */
char* propertyToJSON(const Property* prop) {

  if(prop == NULL) {
    return NULL;
  }

  /*Declare variables*/
  char *valStr = strListToJSON(prop->values);
  char *paramStr = paramListToJSON(prop->parameters);
  char *propJSON = malloc(sizeof(char) * (strlen(prop->group) + strlen(prop->name) + strlen(valStr) + strlen(paramStr)) + 100 );
  propJSON[0] = '\0';

  sprintf(propJSON, "{\"group\":\"%s\",\"name\":\"%s\",\"values\":%s,\"parameters\":%s}", prop->group, prop->name, valStr, paramStr);

  if(valStr != NULL) {
    free(valStr);
    valStr = NULL;
  }

  if(paramStr != NULL) {
    free(paramStr);
    paramStr = NULL;
  }

  return propJSON;
}

/* Take in a parameter struct and convert to a json object */
char* paramToJSON(Parameter *param) {
  if(param == NULL) {
    return NULL;
  }

  /*Declaring variables*/
  char *strJSON = malloc((sizeof(char) * (strlen(param->name) + strlen(param->value)))  + 200);
  char *s = NULL;
  int i = 0;
  int j = 0;

  /* If the value has quotes get rid of them */
  if(param->value[0] == '\"') {
    int len = strlen(param->value);
    s = malloc(sizeof(char) * len + 10);
    for(i = 1; i < len; i++) {
      s[j] = param->value[i];
      j++;
    }
    s[j-1] = '\0';

     sprintf(strJSON, "{\"name\":\"%s\",\"value\":\"%s\"}", param->name, s);
  } 
  else {
    sprintf(strJSON, "{\"name\":\"%s\",\"value\":\"%s\"}", param->name, param->value);
  }

  return strJSON;
}

/* Take a parameter list and create json object*/
char *paramListToJSON(List *param) {
  if(param == NULL) {
    return NULL;
  }

  /* Declaring variables */
  List *tmpList = param;
  char *JSONStr = NULL;
  void *elem;

  JSONStr = malloc(sizeof(char) + 10);
  strcpy(JSONStr, "[");

  /* Iterate through the parameters list and get the JSON string of the parameter*/
  ListIterator iter = createIterator(tmpList);
  while((elem = nextElement(&iter)) != NULL) {
    Parameter *tmpParam = (Parameter*)elem;
    char* buff = paramToJSON(tmpParam);
    JSONStr = realloc(JSONStr, sizeof(char) * (strlen(buff) + strlen(JSONStr) + 50));
    strcat(JSONStr, buff);
    strcat(JSONStr, ",");
  }

  int i = strlen(JSONStr);

  if(strcmp(JSONStr, "[") == 0) {
    JSONStr = realloc(JSONStr, sizeof(char) + 10);
    strcpy(JSONStr, "[]");
  } else {
    JSONStr[i-1] = ']';
  }
  
  return JSONStr;
}

/* Add the new property */
bool addNewProperty(char *fileName, char *propName, char *propVal) {
  
  if(fileName == NULL || propName == NULL || propVal == NULL) {
    return false;
  }

  /* Declaring variables */
  Card *newCard = NULL;
  char *valueNode = malloc(sizeof(char) + strlen(propVal) + 20);
  strcpy(valueNode, propVal);

  if(createCard(fileName, &newCard) == OK) {

    /*Initialize property */
    Property *newProp = malloc(sizeof(Property));
    newProp->name = malloc(sizeof(char) * (strlen(propName) + 10));
    newProp->group = malloc(sizeof(char) * 5);
    newProp->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
    newProp->values = initializeList(&valueToString, &deleteValue, &compareValues);

    strcpy(newProp->name, propName);
    newProp->group[0] = '\0';
    insertBack(newProp->values, (void*)valueNode);

    addProperty(newCard, newProp);

    if(validateCard(newCard) == OK) {
      if(writeCard(fileName, newCard) == OK) {
        return true;
      }
    } else {
      return false;
    }
  } else {
    free(valueNode);
    return false;
  }

  free(valueNode);
  return false;
}

/* This function updates the card with the new edited value */
bool editVal(int index, char *value, char *fileName) {
  if(value == NULL) {
    return false;
  }

  /* Declaring variables */
  Card *newCard = NULL;
  char *val = NULL;
  void *elem;
  bool hasComma = false;
  int i = 0;

  /*Can only do it for single value properties*/
  for(i = 0; i < strlen(value); i++) {
    if(value[i] == ',') {
      hasComma = true;
    }
  }

  if(hasComma == true) {
    printf("Has comma, can only be a single value\n");
    return false;
  }

  if(createCard(fileName, &newCard) == OK) {
    if(index == -1) {
      /*Clear list and reinitialize it with new values*/
      clearList(newCard->fn->values);
      newCard->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
      val = malloc(sizeof(char) * strlen(value) + 30);
      strcpy(val, value);
      insertBack(newCard->fn->values, (void*)val);

      /* Validate the new card */
      if(validateCard(newCard) == OK) {
        if(writeCard(fileName, newCard) == OK) {
          return true;
        }
      }
    } else {
      // int ind = atoi(index);
      i = 0;
      /* Iterate through the list and only change the index values */
      ListIterator iterOpProp = createIterator(newCard->optionalProperties);
      while((elem = nextElement(&iterOpProp)) != NULL) {
        Property *tmpProp = (Property *)elem;

        if(i == index) {
          clearList(tmpProp->values);
          tmpProp->values = initializeList(&valueToString, &deleteValue, &compareValues);
          char *v = malloc(sizeof(char) * strlen(value) + 30);
          strcpy(v, value);
          insertBack(tmpProp->values, (void*)v);
        }
        i++;
      }

      if(validateCard(newCard) == OK) {
        if(writeCard(fileName, newCard) == OK) {
          return true;
        }
      }
    }
  }
  return false;
}