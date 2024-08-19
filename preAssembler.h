#ifndef PRE_ASEMBLER_H
#define PRE_ASEMBLER_H

void initConstantNames();

/* Add a new constant name to the array */
void addConstantName(char *name);

/* Check if the given string matches any constant names
    return 1 if there is
    return 0 if not
notes: this function are help to verify valid macr name.
 */
int argStrContainConstantNames(char *argStr);

/* Free all constant names */
void freeConstantNames(void);

int preAssembler(char* fileName);

#endif /* PRE_ASEMBLER_H */