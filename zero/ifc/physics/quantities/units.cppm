/**
* @brief Predefined physical units based on the definitions
 * provided by the international system
*/

export module physics:units;

import std;

import concepts;
import :ratios;
import :dimensions;
import :units.symbols;
import :units.detail;

using namespace units::__detail;

export namespace zero::physics {
    /* Base units */
    template<Ratio r, Symbol s>
    struct base_unit {
        using ratio = r;
        using symbol = s;
    };

    template <typename T>
    concept BaseUnit =
        Ratio<typename T::ratio> &&
        Symbol<typename T::symbol> &&
    requires {
        typename T::ratio;
        typename T::symbol;
    };

    struct Kilogram: public mass<1>, public base_unit<kilo, kg> {};
    struct Hectogram: public mass<1>, public base_unit<hecto, hg> {};
    struct Meter: public length<1>, public base_unit<root, m> {};
    struct Second: public time<1>, public base_unit<second, s> {};
    struct Hour: public time<1>, public base_unit<hour, h> {};

    /* Derived units */
    template <typename DerivedDim, BaseUnit... BaseUnits>
        requires (DerivedDimension<DerivedDim>)
    struct derived_unit {
        using derived_dimension = DerivedDim;
        using units = std::tuple<BaseUnits...>;

        static constexpr double dimensionality = []<std::size_t... Is>(std::index_sequence<Is...>) {
            using dm_exp = dimensions_exponents<typename DerivedDim::dimensions>;
            return (1.0 * ... * consteval_power(BaseUnits::ratio::value, dm_exp::value[Is]));
        } (std::make_index_sequence<DerivedDim::total_dimensions>{});
    };

    template <typename T, std::size_t... Is>
    concept DerivedUnit = requires {
        typename T::units;
        typename T::derived_dimension;
        T::dimensionality;
    } && (std::is_base_of_v<derived_unit<typename T::derived_dimension, std::tuple_element_t<Is, typename T::units>>, T> && ...);

    struct MetersPerSecond :
        public derived_unit<
            speed,
            base_unit<root, m>,
            base_unit<second, s>
        >
    {};

    struct KilometersPerHour :
        public derived_unit<
            speed,
            base_unit<kilo, km>,
            base_unit<hour, h>
        >
    {};
}
