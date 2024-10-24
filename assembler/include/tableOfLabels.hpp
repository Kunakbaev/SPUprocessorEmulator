#ifndef ASSEMBLER_INCLUDE_TABLE_OF_LABELS_HPP
#define ASSEMBLER_INCLUDE_TABLE_OF_LABELS_HPP

enum TableOfLabelsErrors {
    TABLE_OF_LABELS_ERROR_STATUS_OK               = 0,
    TABLE_OF_LABELS_ERROR_INVALID_ARGUMENT        = 1,
    TABLE_OF_LABELS_ERROR_LABEL_NOT_FOUND         = 2,
    TABLE_OF_LABELS_INVALID_NUM_OF_CODE_LINE_ARG  = 3,
    TABLE_OF_LABELS_ERROR_MEMORY_ALLOCATION_ERROR = 4,
    TABLE_OF_LABELS_ERROR_LABEL_WAS_NOT_DELCLARED = 5,
    TABLE_OF_LABELS_ERROR_LABEL_ALREADY_EXISTS    = 6,
};

struct Label {
    const char* labelName;
    int         codeLineInd;
};

TableOfLabelsErrors constructTableOfLabels();
const char* getTableOfLabelsErrorMessage(TableOfLabelsErrors error);
TableOfLabelsErrors addLabelName(const Label* label);

// numOfCodeLine can be -1, label not found
TableOfLabelsErrors printAllLabels();
TableOfLabelsErrors checkAllLabelsAreDeclared();
TableOfLabelsErrors getNumOfCodeLineByLabel(const char* labelName, Label* label);
TableOfLabelsErrors destructTableOfLabels();

#endif
