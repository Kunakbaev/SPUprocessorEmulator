

#include "../include/tableOfLabels.hpp"
#include "../../common/include/errorsHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, TABLE_OF_LABELS_ERROR_INVALID_ARGUMENT, getTableOfLabelsErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getTableOfLabelsErrorMessage, TABLE_OF_LABELS_ERROR_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getTableOfLabelsErrorMessage)

const size_t MAX_NUM_OF_LABELS = 1 << 10;

// array of strings (labels)
Label* labels = {};
int numOfLabels = 0;

TableOfLabelsErrors constructTableOfLabels() {
    labels = (Label*)calloc(MAX_NUM_OF_LABELS, sizeof(*labels));
    IF_NOT_COND_RETURN(labels != NULL,
                       TABLE_OF_LABELS_ERROR_MEMORY_ALLOCATION_ERROR);
    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

const char* getTableOfLabelsErrorMessage(TableOfLabelsErrors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case TABLE_OF_LABELS_ERROR_STATUS_OK:
            return "No erros in table of labels, everything is valid.\n";
        case TABLE_OF_LABELS_ERROR_INVALID_ARGUMENT:
            return "Table of labels error: invalid argument (possibly set to NULL).\n";
        case TABLE_OF_LABELS_ERROR_LABEL_NOT_FOUND:
            return "Table of labels error: label not found.\n";
        case TABLE_OF_LABELS_INVALID_NUM_OF_CODE_LINE_ARG:
            return "Table of labels error: invalid num of code line arg (too big or too small, less than zero).\n";
        case TABLE_OF_LABELS_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Table of labels error: couldn't allocate memory.\n";
        case TABLE_OF_LABELS_ERROR_LABEL_ALREADY_EXISTS:
            return "Table of labels error: label already exists.\n";
        case TABLE_OF_LABELS_ERROR_LABEL_WAS_NOT_DELCLARED:
            return "Table of labels error: some labels occured in jump or call command, but was not declared anywhere else in code.\n";

        default:
            return "Unknown error.\n";
    }
}

static TableOfLabelsErrors doesLabelAlreadyExist(const Label* label, Label** result) {
    IF_ARG_NULL_RETURN(label);
    IF_ARG_NULL_RETURN(result);

    *result = NULL;
    for (size_t labelInd = 0; labelInd < numOfLabels; ++labelInd) {
        if (strcmp(labels[labelInd].labelName, label->labelName) == 0) {
            *result = &labels[labelInd];
            LOG_DEBUG("found label with same name");
            return TABLE_OF_LABELS_ERROR_STATUS_OK;
        }
    }

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

TableOfLabelsErrors checkAllLabelsAreDeclared() {
    for (size_t labelInd = 0; labelInd < numOfLabels; ++labelInd) {
        IF_NOT_COND_RETURN(labels[labelInd].codeLineInd != -1,
                           TABLE_OF_LABELS_ERROR_LABEL_WAS_NOT_DELCLARED);
    }

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

TableOfLabelsErrors addLabelName(const Label* label) {
    IF_ARG_NULL_RETURN(label);

    Label* prevPtr = NULL;
    IF_ERR_RETURN(doesLabelAlreadyExist(label, &prevPtr));
    LOG_DEBUG_VARS(prevPtr, label->codeLineInd, label->labelName);
    if (prevPtr != NULL) { // label was already added before, so we don't need to add it again
        if (label->codeLineInd == -1) {
            return TABLE_OF_LABELS_ERROR_STATUS_OK;
        }

        IF_NOT_COND_RETURN(prevPtr->codeLineInd == -1 ||
                           prevPtr->codeLineInd == label->codeLineInd,
                           TABLE_OF_LABELS_ERROR_LABEL_ALREADY_EXISTS);

        *prevPtr = *label; // ASK: is this ok?
        return TABLE_OF_LABELS_ERROR_STATUS_OK;
    }

    LOG_WARNING("adding new label");
    labels[numOfLabels++] = *label;
    LOG_DEBUG_VARS(numOfLabels, label->codeLineInd, label->labelName);

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

TableOfLabelsErrors printAllLabels() {
    for (size_t labelInd = 0; labelInd < numOfLabels; ++labelInd) {
        LOG_DEBUG_VARS(labelInd, labels[labelInd].labelName, labels[labelInd].codeLineInd);
    }

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

TableOfLabelsErrors getNumOfCodeLineByLabel(const char* labelName, Label* label) {
    IF_ARG_NULL_RETURN(labelName);
    IF_ARG_NULL_RETURN(label);

    *label = {};
    for (size_t labelInd = 0; labelInd < numOfLabels; ++labelInd) {
        if (strcmp(labels[labelInd].labelName, labelName) == 0) {
            *label = labels[labelInd];
            return TABLE_OF_LABELS_ERROR_STATUS_OK;
        }
    }

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

TableOfLabelsErrors destructTableOfLabels() {
    FREE(labels);
    numOfLabels = 0;

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}
