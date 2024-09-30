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

// localTrsMatrix // prev랑 비교후 갱신/ 갱신시 islocal머시기 true 아니면 false
// worldTrsMatrix
// bool islocaltrschanged 이거 활성화시 시 월드매트릭스 갱신. isworldtrschanged 이거 활성화
// bool isworldtrschanged 부모가 local 업데이트 or 부모 world 변경시 이거 true. worldtrs변경.


// func LocalToWorld_Position
// func LocalToWorld_Direction

// getForward
// getRight
// getUp

// setForward Right랑 Cross후 Up 갱신
// setRight Forward랑 Cross후 Up 갱신
// setUp Farward랑 Cross

// LookAt(target, upvector)