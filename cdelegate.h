/*!
 * \file	cdelegate.h
 * \date	16.03.2016
 * \author	Sebastian Iwaniec	< sebastian.iwaniec@zmt.tarnow.pl >
 * \brief	Deklaracja i definicja typu delegata
 */
#ifndef CDELEGATE_H
#define CDELEGATE_H

#include <iostream>
/*!
 * \class CDelegate
 * \brief Klasa delegata do wywoływania metod innych klas i funkcji globalnych
 * \par Opis:
 * Meta klasa która służy do tworzenia obiektów delegata który może wywołać funkcję globalną lub metodę klasy.
 * Sposób działania jest następujący, najpierw tworzymy obiekt delegata któremu podajemy w szablonie typ zwracany
 * metody lub funkcji którą ma wywołać i parametry które ta metoda bądź funkcja przyjmuje:
 * "CDelegate<int, char*, int>delegat;" <-- delegat dla funkcji która zwraca int i przyjmuje wskaźnik char oraz int np. int printText(char *buffer, int size);
 *
 * Następnie przypisujemy funkcję lub metodę delegatowi
 * "delegat.Bind<&printText>();" <-- przypisanie funckji globalnej do delegata
 * "delegat.Bind<MojaKlasa, &MojaKlasa::printText>(wskaznikNaObiejkKlasy);" <-- przypisanie metody klady do delegata
 *
 * Wywołanie metody lub funckji globalnej wykonujemy następująco:
 * "delegat.Invoke(bufor, rozmiarBufora);" <-- wywołanie przez fuunkcję Invoke
 * "delegat(bufor, rozmiarBufora);" <-- wywołanie przez przeładowany operator ()
 *
 * Delegaty można porównywać żeby sprawdzić czy czasem nie wskazują na tą samą funkcję lub metodę:
 * "if (delegat1 == delegat2) {}"
 *
 * \par Parametry szablonu:
 * \tparam return_type Typ zwwracany przez funckję którą ma wywoływać delegat
 * \tparam params Lista parametrów które przyjmuje funkcja którą ma wywoływać delegat
 */
template <typename return_type, typename... params>
class CDelegate
{
    typedef void* InstancePtr;
    typedef return_type (*InternalFunction)(InstancePtr, params...);
    typedef std::pair<InstancePtr, InternalFunction> Stub;

    /*!
     * \brief Wywoływanie funkcji globalnej przez delegata
     *
     * \par Parametry szablonu:
     * \tparam Function wskaźnik na funckję
     *
     * \par Atrybuty:
     * \param[in] xs lista parametrów uruchomieniowych
     *
     */
    template <void (*Function)(params...)>
    static inline return_type FunctionStub(InstancePtr, params... xs)
    {
        // we don't need the instance pointer because we're dealing with free functions
        return (Function)(xs...);
    }

    /*!
     * \brief Wywoływanie metody klasy przez delegata
     *
     * \par Parametry szablonu:
     * \tparam C Klasa w której jest metoda
     * \tparam Function wskaźnik na metodę
     *
     * \par Atrybuty:
     * \param[in] instance wskaźnik na obiekt klasy dla którego jest wywoływana metoda
     * \param[in] xs lista parametrów uruchomieniowych
     *
     */
    template <class C, void (C::*Function)(params...)>
    static inline return_type ClassMethodStub(InstancePtr instance, params... xs)
    {
        // cast the instance pointer back into the original class instance
        return (static_cast<C*>(instance)->*Function)(xs...);
    }

public:
    CDelegate(void)
        : m_stub(nullptr, nullptr)
    {
    }
    /*!
     * \brief Przeładowanie operatora () na wywoanie funkcji Invoke
     *
     * \par Atrybuty:
     * \param[in] xs lista parametrów uruchomieniowych
     */
    return_type operator()(params ...xs)
    {
        return Invoke(xs...);
    }

    /*!
     * \brief Przeładowanie operatora == w celu poruwnywania obiektów
     *
     * \par Atrybuty:
     * \param[in] other obiekt który należy poddać testowi
     *
     * \par Wartości zwracane:
     * \return 1 delegaty wskazują na te same funkcjie i obiekty
     * \return 0 delegaty nie wskazują na te same funkcjie i obiekty
     */
    bool operator==(const CDelegate &other) const
    {
        return (m_stub.first == other.m_stub.first
                && m_stub.second == other.m_stub.second);
    }

    /*!
    * \brief Metoda podłączania do delegata funkcji globalnej
    *
    * \par Parametry szablonu:
    * \tparam Function Wskaźnik na funckję globalną
    */
    template <void (*Function)(params...)>
    void Bind(void)
    {
        m_stub.first = nullptr;
        m_stub.second = &FunctionStub<Function>;
    }

    void UnBind()
    {
        m_stub.first = nullptr;
        m_stub.second = nullptr;
    }

    /*!
    * \brief Metoda podłączenia do delegata metody innej klasy
    *
    * \par Parametry szablonu:
    * \tparam C Klasa z której jest metoda którą można wywołać
    * \tparam Function Metoda uruchamiana w klasie
    * \par Argumenty:
    * \param[in] instance wskaźnik na obiekt dla którego am być wywołana metoda
    */
    template <class C, void (C::*Function)(params...)>
    void Bind(C* instance)
    {
        m_stub.first = instance;
        m_stub.second = &ClassMethodStub<C, Function>;
    }

    /*!
     * \brief Wywołanie funkcji globalnej lub metody klasy
     *
     * \par Atrybuty:
     * \param[in] xs lista parametrów uruchomieniowych
     */
    return_type Invoke(params... xs) const
    {
        if (m_stub.second == nullptr)
        {
            std::cout << " [CDelegate] Brak funkcji do wywołania!" << std::endl;
            return;
        }
        return m_stub.second(m_stub.first, xs...);
    }

private:
    Stub m_stub;
};
#endif // CDELEGATE_H
