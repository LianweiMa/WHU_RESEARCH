#ifndef _WHU_VECTOR_2014_07_06_MLW
#define _WHU_VECTOR_2014_07_06_MLW
#include <stdio.h>
#include <vector>
//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* ɾ������ָ��λ�õ�Ԫ��                                                             */
/* author:malianwei,  date:2014-07-06                                               */
/* [in] vector :��������                                                                     */
/* [in] index :ָ��ɾ��Ԫ�ص�λ��                                                 */
/* [return] ɾ���ɹ�����true,���򷵻�false                                */
/************************************************************************/
template <typename T>
bool DelVectorElement(T &vector,const unsigned int &index)
{
	if (index>=vector.size()){ printf("\n\terror:\n\t\tvector: index position out of range:index(%d)vector size(%d)\n",index,vector.size());return false;}
	T::iterator itr=vector.begin();
	vector.erase(itr+index);
	return true;
}
/************************************************************************/
/* �������ָ��λ��Ԫ��                                                             */
/* author:malianwei,  date:2014-07-06                                            */
/* [in] vector :��������                                                                     */
/* [in] index :ָ��ɾ��Ԫ�ص�λ��                                                 */
/* [return] ɾ���ɹ�����true,���򷵻�false                                */
/************************************************************************/
template <typename T1,typename T2>
bool InsertVectorElement(T1 &vector,const unsigned int &index,T2 &insertElem)
{
	if (index>vector.size()){ printf("\n\terror:\n\t\tvector: index position out of range:index(%d)vector size(%d)\n",index,vector.size());return false;}
	if(index==vector.size()) {vector.push_back(insertElem);return true;}
	T1::iterator itr=vector.begin();
	vector.insert(itr+index,insertElem);
	return true;
}
#endif