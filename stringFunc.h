#ifndef STRING_FUNC_H
#define STRING_FUNC_H

/* Duplicate a string */
char* strduppp(const char* str);

/* Remove multiple spaces. Example: change "helo    world!" to "helo world!" */
void removeExtraSpaces(char *str);

/* Remove leading and trailing spaces from a string. Example: "   helo world!  " to "helo world!" */
void removeLeadingSpacesAndTrailing(char *str);

/* Remove all spaces from a string */
void removeSpaces(char *str);

/* Takes a filename and a new file extension (file type) as inputs, and returns a new filename with the specified file extension*/
char *convertFile(char *filename, char *newFileType);

int hasMultipalCommas(char *str);

#endif /* STRING_FUNC_H */

