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
	//enum�� ������ Ÿ�ӿ� ������
	enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	//��������� ����
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
//ó�� Tail::Tail... �������ϱ� ���� �����ݾ�? �׸��� ���߿� �� ������� �ް���
//�ε����� ã�� ����

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

//index 2�� �ش��ϴ� Ŭ������?  �̶�� ��������� ã����
//TypeList<Mage, Knight, Archer> index = 2?
//TypeList<Knight, Archer> index = 1?
//TypeList<Archer> index = 0? 

//TypeAt<TypeList<Mage, Knight, Archer>, 3>::Result;
//	TypeAt<TypeList<Knight, Archer>, 2>::Result;
//		TypeAt<TypeList<Archer>, 1>::Result;
//			TypeAt<TypeList<>, 0>::Result; //�ش��ϴ� �Լ��� ��� Error 

	//TypeAt
	//using TL = TypeList<Mage, Knight, Archer>;
	//TypeAt<TL, 0>::Result whoAMI6;	//Mage
	//TypeAt<TL, 1>::Result whoAMI7;	//Knight
	//TypeAt<TL, 2>::Result whoAMI8;	//Archer
	//TypeAt<TL, 3>::Result whoAMI8;	//error ��� ������ �ݺ��Ǽ� �ᱹ �ȵȴٰ� �ϴ°ǰ�?

#pragma endregion

////////////////////////////IndexOf/////////////////////////////

#pragma  region IndexOf

//TypeAt�� �ݴ�� �ش��ϴ� Ŭ������ ���� index ã��

//						index
template<typename TL, typename T>
struct IndexOf;

//���ʿ� TypeList�� T��, IndexOf�� T�� ���ƾ����� value = 0; �� ������ �Ǵϱ�
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
	//��ģ���� ���������� ��������� ���� �������� �ǰ�?
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
//From type -> To type���� ��ȯ ����?
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
		//From -> To �� Ÿ�� ��ȯ ����? �ȵǸ� �������� ���� �ִ� �������� ��ȯ

		//MakeFrom()�� ȣ���Ѵ��� FromŸ���� To�� �ɽ��� �����ϸ� Small, �ȵǸ� Big ���� ������ �ٸ��� ��
		//�� �����Ϸ��� �˸��� �Լ��� ã�Ƽ� ���� ��Ų�ٴ� ��Ŀ���� �̿�
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

//���� ��ü�� �ϳ��� Ŭ����ȭ ��Ŵ
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
		using FromType = typename TypeAt<TL, i>::Result;	//i index�� �ش��ϴ� Ÿ��
		using ToType = typename TypeAt<TL, j>::Result;		//j index�� �ش��ϴ� Ÿ��

		if (Conversion<const FromType*, const ToType*>::exists) //��ȯ ����?
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>()); //��������� ������ ����
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>)
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{
		//������ �뵵
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	//table�� �����?
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];


//TypeConversion�� ���� �̿��ϱ����� �뵵
//�� �Լ����� ����ҷ��� Ŭ������ TL, typeID�� ������ ���� ��� ����
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