#pragma once


#define D2R 0.017453292519943295f
#define R2D 57.29577951308232f

namespace std
{
	inline const float to_radian() { return D2R; }
	inline const float to_degree() { return R2D; }
	/// <summary>
	/// ��ȣ ���� -1, 0, 1
	/// </summary>
	/// <param name="x">int ��</param>
	/// <returns></returns>
	inline char sign(const int& x) { return (x > 0) - (x < 0); }
}