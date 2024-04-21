#pragma once
#include <iostream>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include "Cell.hpp"

//Type list used to tell tissue classes how much memory(and at what indices)
//to allocate for different types of cells at compile time

//typelist
template <class... CellTypes>   
struct TissueCellTypeList {};   

//utility to verify that the given type is derived form StemCell
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, typename... Types>                                                                       //
constexpr bool TypesDerivedFromStemCell = (... && std::is_base_of_v<StemCell<T>, Types>);                   //
                                                                                                            //
template <class T, class... Types>                                                                          //
constexpr void VerifyTypesDerivedFromStemCell(TissueCellTypeList<Types...> types)                           //
{                                                                                                           //
    static_assert(TypesDerivedFromStemCell<T, Types...>, "One or more type(s) not derived from StemCell");  //
}                                                                                                           //
                                                                                                            //
template <class T, typename Type>                                                                           //
constexpr bool TypeDerivedFromStemCell = std::is_base_of_v<StemCell<T>, Type>;                              //
                                                                                                            //
template <class T, class Type>                                                                              //
constexpr void VerifyTypeDerivedFromStemCell()                                                              //
{                                                                                                           //
    static_assert(TypeDerivedFromStemCell<T, Type>, "Type isn't derived from StemCell");                    //
}                                                                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//print the types in typelist
template <class T, class... Types>
constexpr void TissueCellTypeList_Print(TissueCellTypeList<Types...> types)
{
    VerifyTypesDerivedFromStemCell<T>(types);
    (void)std::initializer_list<int>{
        (std::cout << typeid(Types).name() << '\n', 0)...
    };
}

//append a type to typelist
template <class T, class NewType, class... Types>
constexpr auto TissueCellTypeList_Append(TissueCellTypeList<Types...> types)
{
    VerifyTypeDerivedFromStemCell<T, NewType>();
    return TissueCellTypeList<Types..., NewType>{};
}

//get the type of the cell at index
//////////////////////////////////////////////////////////////////////////////////
template <size_t Index, class Type>                                             //
struct CellTypeAt;                                                              //
                                                                                //
template <size_t Index, class... Types>                                         //
struct CellTypeAt<Index, TissueCellTypeList<Types...>>                          //
{                                                                               //
    using type = typename std::tuple_element<Index, std::tuple<Types...>>::type;//
};                                                                              //
                                                                                //
template<class T, size_t Index, class... Types>                                 //
constexpr auto TissueCellTypeList_CellAt(TissueCellTypeList<Types...> types)    //
{                                                                               //
    VerifyTypesDerivedFromStemCell<T>(types);                                   //
    return typename CellTypeAt<Index, TissueCellTypeList<Types...>>::type{};    //
}                                                                               //
//////////////////////////////////////////////////////////////////////////////////


//compile time for loop
///////////////////////////////////////////////////////////////////////////////////
template <size_t Iterations>                                                     //
struct TissueCellTypeList_ConstexprForLoop                                       //
{                                                                                //
    template <class Function, size_t... Indices>                                 //
    constexpr void For(Function function, std::index_sequence<Indices...>)       //
    {                                                                            //
        (function(Loop<Indices>{}), ...);                                        //
    }                                                                            //
                                                                                 //
    template <size_t N, class Function>                                          //
    constexpr void For(Function function)                                        //
    {                                                                            //
        For(function, std::make_index_sequence<N>());                            //
    }                                                                            //
                                                                                 //
    template <size_t Iter>                                                       //
    struct Loop                                                                  //
    {                                                                            //
        static constexpr size_t I = Iter;                                        //
    };                                                                           //
};                                                                               //
                                                                                 //
template <size_t Iterations, class Function>                                     //
constexpr void TissueCellTypeList_ConstexprForEach(Function function)            //
{                                                                                //
    TissueCellTypeList_ConstexprForLoop<Iterations> looper;                      //
    looper.For<Iterations>(std::forward<Function>(function));                    //
}                                                                                //
///////////////////////////////////////////////////////////////////////////////////

//total size that each cell's property values will take in bytes
template<class T_, class... Types>
constexpr size_t TissueCellTypeList_SizeBytes(TissueCellTypeList<Types...> types)
{
    VerifyTypesDerivedFromStemCell<T_>(types);

    size_t size = 0;
    TissueCellTypeList_ConstexprForEach<sizeof...(Types)>
    (
            [&size, types](auto index)
            {
            size +=
                CellTypeAt<
                        decltype(index)::I,
                        TissueCellTypeList<Types...>
                    >::type::NumProperties * sizeof(T_);
            }
    );

    return size;
}

//number of types in the typelist
template<class T, class... Types>
constexpr size_t TissueCellTypeList_Size(TissueCellTypeList<Types...> types)
{
    VerifyTypesDerivedFromStemCell<T>(types);
    return sizeof...(Types);
}