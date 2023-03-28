#pragma once
#include "Types.h"

////////////////////////////TypeList/////////////////////////////

#pragma region TypeList
template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};

//TypeList
//TypeList<Mage, Knight>::Head whoAMI;		//Mage
//TypeList<Mage, Knight>::Tail whoAMI2;		//Knigh
//			head,		tail
//TypeList<Mage, TypeList<Knight, Archer>>::Head whoAMI3;			//Mage
//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head whoAMI4;		//Knight
//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail whoAMI5;		//Archer

#pragma endregion

////////////////////////////Length//////////////////////////////

#pragma region Length
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	//enum도 컴파일 타임에 정해짐
	enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	//재귀적으로 더함
	enum { value = 1 + Length<TypeList<U...>>::value };
};

//Length<TypeList<Mage, Knight, Archer>>::value;
//	1 + Length<TypeList<Knight, Archer>>::value;
//		1 + 1 + Length<TypeList<Archer>>::value;
//			1 + 1 + 1 + Length<TypeList<>>::value;
//				1 + 1 + 1 + 0;

//Lenght
//int32 len1 = Length<TypeList<Mage, Knight>>::value;
//int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;

#pragma endregion

////////////////////////////TypeAt//////////////////////////////

#pragma region TypeAt

//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail whoAMI5;
//처럼 Tail::Tail... ㅇㅈㄹ하기 ㅈ나 힘들잖어? 그리고 나중에 더 길어지면 햇갈림
//인덱스로 찾아 보자

template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

//index 2에 해당하는 클래스는?  이라고 재귀적으로 찾아줌
//TypeList<Mage, Knight, Archer> index = 2?
//TypeList<Knight, Archer> index = 1?
//TypeList<Archer> index = 0? 

//TypeAt<TypeList<Mage, Knight, Archer>, 3>::Result;
//	TypeAt<TypeList<Knight, Archer>, 2>::Result;
//		TypeAt<TypeList<Archer>, 1>::Result;
//			TypeAt<TypeList<>, 0>::Result; //해당하는 함수가 없어서 Error 

	//TypeAt
	//using TL = TypeList<Mage, Knight, Archer>;
	//TypeAt<TL, 0>::Result whoAMI6;	//Mage
	//TypeAt<TL, 1>::Result whoAMI7;	//Knight
	//TypeAt<TL, 2>::Result whoAMI8;	//Archer
	//TypeAt<TL, 3>::Result whoAMI8;	//error 계속 무한히 반복되서 결국 안된다고 하는건가?

#pragma endregion

////////////////////////////IndexOf/////////////////////////////

#pragma  region IndexOf

//TypeAt과 반대로 해당하는 클래스가 속한 index 찾기

//						index
template<typename TL, typename T>
struct IndexOf;

//에초에 TypeList의 T랑, IndexOf의 T랑 같아야지만 value = 0; 이 셋팅이 되니까
//							index
//find it
template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

//cant find
template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
	//이친구도 마찬가지로 재귀적으로 들어가서 더해지는 건가?
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };
	enum { value = (temp == -1) ? -1 : temp + 1 };
};

//IndexOf<TypeList<Mage, Knight, Archer>, Archer>::value;	
//	IndexOf < TypeList<Knight, Archer>, Archer>::value;
//		IndexOf<TypeList<Archer>, Archer>::value == 0;

//IndexOf<TypeList<Mage, Knight, Archer>, Dog>::value;
//	IndexOf<TypeList<Knight, Archer>, Dog>::value;
//		IndexOf<TypeList<Archer>, Dog>::value;	
//			IndexOf<TypeList<>, Dog>::value == -1;


//IndexOf
//using TL = TypeList<Mage, Knight, Archer>;
//int32 index1 = IndexOf<TL, Mage>::value;
//int32 index2 = IndexOf<TL, Archer>::value;
//int32 index3 = IndexOf<TL, Dog>::value;

#pragma endregion

////////////////////////////Conversion///////////////////////////

#pragma region Conversion
//From type -> To type으로 변환 가능?
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
		//From -> To 로 타입 변환 가능? 안되면 가변길이 인자 있는 버전으로 변환

		//MakeFrom()을 호출한다음 From타입을 To로 케스팅 성공하면 Small, 안되면 Big 으로 같은지 다른지 비교
		//즉 컴파일러가 알맞은 함수를 찾아서 실행 시킨다는 메커니즘 이용
	};
};

//Conversion<Player, Knight>::exists;
//sizeof(Test(Player)) == sizeof(Knight);
//	if (Knight & = player)
//		return true;(X)
//	else
//		return false;(O)

//Conversion
//bool canConvert1 = Conversion<Player, Knight>::exists;
//bool canConvert2 = Conversion<Knight, Player>::exists;
//bool canConvert3 = Conversion<Knight, Dog>::exists;

#pragma endregion

////////////////////////////TypeCast///////////////////////////

#pragma region TypeCast

//숫자 자체를 하나의 클래스화 시킴
template<int32 v>
struct Int2Type
{
	enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
		//for (int i = 0; i < length; i++)
		//{
		//	for (int j = 0; j < length; j++)
		//	{
		//		using FromType = typename TypeAt<TL, i>::Result;
		//		using ToType = typename TypeAt<TL, j>::Result;
		//
		//		if (Conversion<const FromType*, const ToType*> ::exists)
		//			s_convert[i][j] = true;
		//		else
		//			s_convert[i][j] = false;
		//	}
		//}
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		//https://woo-dev.tistory.com/33
		using FromType = typename TypeAt<TL, i>::Result;	//i index에 해당하는 타입
		using ToType = typename TypeAt<TL, j>::Result;		//j index에 해당하는 타입

		if (Conversion<const FromType*, const ToType*>::exists) //변환 가능?
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>()); //재귀적으로 다음꺼 실행
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>)
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{
		//끝내기 용도
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	//table로 만든다?
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];


//TypeConversion을 쉽게 이용하기위한 용도
//이 함수들을 사용할려면 클래스에 TL, typeID를 셋팅해 놔야 사용 가능
template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<To>(ptr);

	return nullptr;

	//Player* player = new Knight();
	//
	//Knight* knight = TypeCast<Knight*>(player);
	//
	//delete player;
}

//shared_ptr version
template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(ptr);

	return nullptr;

	//	shared_ptr<Player> player = MakeShared<Knight>();
	//
	//	shared_ptr<Archer> archer = TypeCast<Archer>(player);

}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);

	//Player* player = new Knight();
	//
	//bool canCast = CanCast<Knight*>(player);
	//
	//delete player;
}

//shared_ptr version
template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);

	//	shared_ptr<Player> player = MakeShared<Knight>();
	//
	//	bool canCast = CanCast<Mage>(player);
}

#pragma endregion

#define DECLARE_TL		using TL = TL; int32 _typeId;
#define INIT_TL(Type)	_typeId = IndexOf<TL, Type>::value;