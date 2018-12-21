import QtQuick 2.0

import INGESCAPE 1.0

/**
 * Custom text field validator that allows integer inputs and checks the top/bottom bounds.
 * Basically a IntValidator from Qt with our custom bounds, limitating inputs to values that fit into a 31-bit integer.
 */
IntValidator {
    top:    NumberConstants.MAX_INTEGER
    bottom: NumberConstants.MIN_INTEGER
}
