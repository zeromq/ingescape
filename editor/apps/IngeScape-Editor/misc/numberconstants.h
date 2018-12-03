#ifndef NUMBERCONSTANTS_H
#define NUMBERCONSTANTS_H

#include <QObject>

#include <limits>

#include <I2PropertyHelpers.h>

/**
 * @brief Holds the numeric bounds for 32-bit signed integers and 64-bit floating points values.
 * Used from QML but could also be used from C++.
 */
class NumberConstants : public QObject
{
    Q_OBJECT

    /**
     * 32-bit signed integer upper bound value
     */
    Q_PROPERTY ( int    MAX_INTEGER READ MAX_INTEGER CONSTANT )
    /**
     * 32-bit signed integer lower bound value
     */
    Q_PROPERTY ( int    MIN_INTEGER READ MIN_INTEGER CONSTANT )
    /**
     * 64-bit floating point upper bound value
     */
    Q_PROPERTY ( double MAX_DOUBLE  READ MAX_DOUBLE  CONSTANT )
    /**
     * 64-bit floating point lower bound value
     */
    Q_PROPERTY ( double MIN_DOUBLE  READ MIN_DOUBLE  CONSTANT )

    public:
        /**
         * @brief Default constructor inehrited from QObject
         * @param parent
         */
        explicit NumberConstants(QObject* parent = nullptr);

        /**
         * @brief Method used to provide a singleton to QML
         * @param engine
         * @param scriptEngine
         * @return
         */
        static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);

        /**
         * @brief Accessor for the upper bound of a 32-bit signed integer
         * @return The 32-bit signed integer upper bound value
         */
        static inline int    MAX_INTEGER() { return _MAX_INTEGER; }

        /**
         * @brief Accessor for the lower bound of a 32-bit signed integer
         * @return The 32-bit signed integer lower bound value
         */
        static inline int    MIN_INTEGER() { return _MIN_INTEGER; }

        /**
         * @brief Accessor for the upper bound of a 64-bit floating point number
         * @return The 64-bit floating point upper bound value
         */
        static inline double MAX_DOUBLE()  { return _MAX_DOUBLE;  }

        /**
         * @brief Accessor for the lower bound of a 64-bit floating point number
         * @return The 64-bit floating point lower bound value
         */
        static inline double MIN_DOUBLE()  { return _MIN_DOUBLE;  }

    private:
        static constexpr int _MAX_INTEGER = std::numeric_limits<int>::max(); // maximum 32bit signed integer value
        static constexpr int _MIN_INTEGER = std::numeric_limits<int>::min(); // minimum 32bit signed integer value

        static constexpr double _MAX_DOUBLE =  std::numeric_limits<double>::max(); // maximum 64bit IEEE-754 (double precision) value (~ 1e+308)
        static constexpr double _MIN_DOUBLE = -std::numeric_limits<double>::max(); // minimum 64bit IEEE-754 (double precision) value (~ -1e+308)

};

QML_DECLARE_TYPE(NumberConstants)

#endif // NUMBERCONSTANTS_H
