

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
            return "No erros in assembler, everything is valid.\n";
        case TABLE_OF_LABELS_ERROR_INVALID_ARGUMENT:
            return "Assembler error: invalid argument (possibly set to NULL).\n";
        case TABLE_OF_LABELS_ERROR_LABEL_NOT_FOUND:
            return "Table of labels error: label not found.\n";
        case TABLE_OF_LABELS_INVALID_NUM_OF_CODE_LINE_ARG:
            return "Table of labels error: invalid num of code line arg (too big or too small, less than zero).\n";
        case TABLE_OF_LABELS_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Assembler error: couldn't allocate memory.\n";

        default:
            return "Unknown error.\n";
    }
}

TableOfLabelsErrors doesLabelAlreadyExist(const Label* label, bool* is) {
    IF_ARG_NULL_RETURN(label);
    IF_ARG_NULL_RETURN(is);

    *is = false;
    for (size_t labelInd = 0; labelInd < numOfLabels; ++labelInd) {
        if (strcmp(labels[labelInd].labelName, label->labelName) == 0) {
            *is = true;
            return TABLE_OF_LABELS_ERROR_STATUS_OK;
        }
    }

    return TABLE_OF_LABELS_ERROR_STATUS_OK;
}

TableOfLabelsErrors addLabelName(const Label* label) {
    IF_ARG_NULL_RETURN(label);

    bool wasLabelBefore = false;
    IF_ERR_RETURN(doesLabelAlreadyExist(label, &wasLabelBefore));
    if (wasLabelBefore) // label was already added before, so we don't need to add it again
        return TABLE_OF_LABELS_ERROR_STATUS_OK;

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
