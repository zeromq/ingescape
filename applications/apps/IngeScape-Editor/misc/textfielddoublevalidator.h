#ifndef TEXTFIELDDOUBLEVALIDATOR_H
#define TEXTFIELDDOUBLEVALIDATOR_H

#include <QDoubleValidator>

/**
 * @brief Custom text field validator to only accept double values inside boundaries.
 *
 * The fact is that QML TextField accepts intermediate inputs in order to accept inputs like "1111." while the user is typing.
 * QDoubleValidator returns QValidator::State::Intermediate for double values that are out of bound.
 * This class override this behavior to prevent the user from typing values that are out of the specified bounds of the validator.
 *
 * More information here
 * @link https://stackoverflow.com/a/35223994/2381553
 */
class TextFieldDoubleValidator : public QDoubleValidator
{
    public:
        /**
         * @brief Default constructor
         * Inherited from QDoubleValidator
         *
         * @param parent
         */
        TextFieldDoubleValidator(QObject* parent = nullptr);

        /**
         * @brief Second constructor
         * Injerited from QDoubleValidator
         *
         * @param bottom
         * @param top
         * @param decimals
         * @param parent
         */
        TextFieldDoubleValidator(double bottom, double top, int decimals, QObject* parent = nullptr);

        /**
         * @brief Methods overriden from QDoubleValidator. Actually tels the TextField if some input is valid or not.
         *
         * @param s The input to validate
         * @param pos The position of the cursor
         * @return A QValidator::State giving the state of the input.
         */
        QValidator::State validate(QString& s, int&) const override;
};

#endif // TEXTFIELDDOUBLEVALIDATOR_H
