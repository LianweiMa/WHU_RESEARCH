#ifndef _WHU_ALGEBRA_2014_07_06_MLW
#define _WHU_ALGEBRA_2014_07_06_MLW
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////
//代数运算
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 计算向量的向量积                                                                     */
/* author:malianwei,  date:2014-07-06                                               */
/* [in] vector1:第一个输入向量                                                                     */
/* [in] vector2:第二个输入向量                                                                     */
/* [return] 返回向量积                                                                  */
/************************************************************************/
template <typename T>
T CorssProduct(const T &vector1,const T &vector2 )
{
	T resultCrossProduct;
	resultCrossProduct.x=vector1.y*vector2.z-vector2.y*vector1.z;
	resultCrossProduct.y=vector2.x*vector1.z-vector1.x*vector2.z;
	resultCrossProduct.z=vector1.x*vector2.y-vector2.x*vector1.y;
	return resultCrossProduct;
}
/************************************************************************/
/* 计算向量的数量积                                                                     */
/* author:malianwei,  date:2014-07-06                                               */
/* [in] vector1:第一个输入向量                                                                     */
/* [in] vector2:第二个输入向量                                                                     */
/* [return] 返回数量积                                                                  */
/************************************************************************/
template <typename T>
double DotProduct(const T &vector1,const T &vector2)
{
	return vector1.x*vector2.x+vector2.y*vector1.y+vector1.z*vector2.z;
}
#endif
