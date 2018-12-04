import QtQuick 2.0

import INGESCAPE 1.0

/**
 * Custom text field validator that allows floating point inputs and checks the top/bottom bounds.
 * DoubleValidator from Qt does not return Invalid on a value that exceeds the given bounds
 * and we would like a validator that prevent the user from exceeding the bounds.
 */
TextFieldDoubleValidator {
    top:    NumberConstants.MAX_DOUBLE
    bottom: NumberConstants.MIN_DOUBLE
}
