#ifndef _WHU_ALGEBRA_2014_07_06_MLW
#define _WHU_ALGEBRA_2014_07_06_MLW
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* ����������������                                                                     */
/* author:malianwei,  date:2014-07-06                                               */
/* [in] vector1:��һ����������                                                                     */
/* [in] vector2:�ڶ�����������                                                                     */
/* [return] ����������                                                                  */
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
/* ����������������                                                                     */
/* author:malianwei,  date:2014-07-06                                               */
/* [in] vector1:��һ����������                                                                     */
/* [in] vector2:�ڶ�����������                                                                     */
/* [return] ����������                                                                  */
/************************************************************************/
template <typename T>
double DotProduct(const T &vector1,const T &vector2)
{
	return vector1.x*vector2.x+vector2.y*vector1.y+vector1.z*vector2.z;
}
#endif
