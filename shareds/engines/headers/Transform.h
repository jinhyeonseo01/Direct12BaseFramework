#pragma once
class Transform
{

};

// localPosition
// localscale
// localRotation (euler)
// 
// prevlocalPosition
// prevlocalscale
// prevlocalRotation

// func localRotationQurt (Qurt)

// func worldposition (Qurt)
// func worldscale 
// func worldRotation (euler)

// localTrsMatrix // prev�� ���� ����/ ���Ž� islocal�ӽñ� true �ƴϸ� false
// worldTrsMatrix
// bool islocaltrschanged �̰� Ȱ��ȭ�� �� �����Ʈ���� ����. isworldtrschanged �̰� Ȱ��ȭ
// bool isworldtrschanged �θ� local ������Ʈ or �θ� world ����� �̰� true. worldtrs����.


// func LocalToWorld_Position
// func LocalToWorld_Direction

// getForward
// getRight
// getUp

// setForward Right�� Cross�� Up ����
// setRight Forward�� Cross�� Up ����
// setUp Farward�� Cross

// LookAt(target, upvector)