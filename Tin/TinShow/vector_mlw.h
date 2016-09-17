#ifndef _WHU_VECTOR_2014_07_06_MLW
#define _WHU_VECTOR_2014_07_06_MLW
#include <stdio.h>
#include <vector>
//////////////////////////////////////////////////////////////////////////
//向量容器操作
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 删除向量指定位置的元素                                                             */
/* author:malianwei,  date:2014-07-06                                               */
/* [in] vector :向量类型                                                                     */
/* [in] index :指定删除元素的位置                                                 */
/* [return] 删除成功返回true,否则返回false                                */
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
/* 添加向量指定位置元素                                                             */
/* author:malianwei,  date:2014-07-06                                            */
/* [in] vector :向量类型                                                                     */
/* [in] index :指定删除元素的位置                                                 */
/* [return] 删除成功返回true,否则返回false                                */
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