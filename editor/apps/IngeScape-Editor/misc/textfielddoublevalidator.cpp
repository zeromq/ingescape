#include "textfielddoublevalidator.h"

/**
 * @brief Default constructor
 * Inherited from QDoubleValidator
 *
 * @param parent
 */
TextFieldDoubleValidator::TextFieldDoubleValidator(QObject* parent)
    : QDoubleValidator(parent)
{
}

/**
 * @brief Second constructor
 * Injerited from QDoubleValidator
 *
 * @param bottom
 * @param top
 * @param decimals
 * @param parent
 */
TextFieldDoubleValidator::TextFieldDoubleValidator(double bottom, double top, int decimals, QObject* parent)
    : QDoubleValidator(bottom, top, decimals, parent)
{
}

/**
 * @brief Methods overriden from QDoubleValidator. Actually tels the TextField if some input is valid or not.
 *
 * @param s The input to validate
 * @param pos The position of the cursor
 * @return A QValidator::State giving the state of the input.
 */
QValidator::State TextFieldDoubleValidator::validate(QString& s, int& /*pos*/) const
{
    if (s.isEmpty() || (s.startsWith("-") && s.length() == 1)) {
        // allow empty field or standalone minus sign
        return QValidator::Intermediate;
    }
    // check length of decimal places
    QChar point = QChar('.');

    s = s.replace(",", point);

    if(s.indexOf(point) != -1) {
        int lengthDecimals = s.length() - s.indexOf(point) - 1;
        if (lengthDecimals > decimals()) {
            return QValidator::Invalid;
        }
    }

    // check range of value
    bool isNumber;
    double value = s.toDouble(&isNumber);
    if (isNumber && bottom() <= value && value <= top()) {
        return QValidator::Acceptable;
    }
    return QValidator::Invalid;
}
