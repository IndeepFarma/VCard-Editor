/*
Student Name: Indeep Farma
Student ID: 1015266
Student Email: ifarma@uoguelph.ca
Assignment 1 CIS 2750
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/* Helper Functions */
bool isColon(char *line, int lineLen);
bool isGroupDelim(char *line, int lineLen);
bool isMultiVal(Property *prop);
bool isMultiValText(char *text);
bool isSemiColon(char *line, int lineLen);
bool isValidDateTime(DateTime *dt);
bool isValidProperty(char* prop);
bool isValidExtention(char *fileName);
bool isValidSex(char *text);
bool hasTime(char *line);
bool validateProp(Property *property, int *nC, int *kC, int *gC, int *pC, int *rC, int *uC);
char *getCardJSON(char *fileName, char *name);
char *getDateString(char *line);
char *getTimeString(char *line);
char *getGroup(char *line, int lineLen);
char *getLine(char *vcard, int *index, int lineLen);
char *getParameter(char *line);
char *getParameterValue(char *line);
char *getProperty(char *line, int lineLen);
char *getStringLeftColon(char *line, int *chCount, int lineLen);
char *getStringRightColon(char *line, int lineLen);
char *validateVCard(char *fileName);
int getContentLineLength(char *line, int *index);
int getIndexColon(char *line, int lineLen);
int getIndexSemiColon(char *line);
int getGroupDelimIndex(char *line, int lineLen);
int getNumOptionalProp(char *fileName);
List *getParametersList(char *line, int lineLen);
List *getValuesList(char *line, int lineLen);
void getIndexQuote(char *string, int *currIndex, int *indexOne, int *indexTwo);

bool createNewVCard(char *fileName, char *FN);
char *getOptionalPropJSON(List *opList);
char* propertyToJSON(const Property* prop);
char* paramToJSON(Parameter *param);
char *paramListToJSON(List *param);
bool addNewProperty(char *fileName, char *propName, char *propVal);
bool editVal(int index, char *value, char *fileName);

/* This function was retrieved from:
https://stackoverflow.com/questions/5820810/case-insensitive-string-comp-in-c */
int strcmpCI(char const *a, char const *b);