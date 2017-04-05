/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

internal render_entity_basis
RotateEntityBasisAroundOrigin(render_entity_basis EntityBasis,
                              r32 RotationX, r32 RotationY, r32 RotationZ)
{
    render_entity_basis Result;
    
    quaternion RotQuatX = RotationQuaterion(V3(1, 0, 0), DegreesToRadians*RotationX);
    quaternion RotQuatY = RotationQuaterion(V3(0, 1, 0), DegreesToRadians*RotationY);
    quaternion RotQuatZ = RotationQuaterion(V3(0, 0, 1), DegreesToRadians*RotationZ);

    quaternion FinalRotQuat = RotQuatZ*RotQuatY*RotQuatX;

    Result.Basis.xAxis = RotateV3(FinalRotQuat, EntityBasis.Basis.xAxis);
    Result.Basis.yAxis = RotateV3(FinalRotQuat, EntityBasis.Basis.yAxis);
    Result.Basis.zAxis = RotateV3(FinalRotQuat, EntityBasis.Basis.zAxis);
    Result.Offset = RotateV3(FinalRotQuat, EntityBasis.Offset);
    
    Result.Basis.ScaleX = EntityBasis.Basis.ScaleX;
    Result.Basis.ScaleY = EntityBasis.Basis.ScaleY;
    Result.Basis.ScaleZ = EntityBasis.Basis.ScaleZ;

    return(Result);
}

internal render_entity_basis
RotateEntityBasisAroundBasis(render_basis Basis, render_entity_basis EntityBasis,
                             r32 RotationX, r32 RotationY, r32 RotationZ)
{
    render_entity_basis Result;
    
    quaternion RotQuatX = RotationQuaterion(Basis.xAxis, DegreesToRadians*RotationX);
    quaternion RotQuatY = RotationQuaterion(Basis.yAxis, DegreesToRadians*RotationY);
    quaternion RotQuatZ = RotationQuaterion(Basis.zAxis, DegreesToRadians*RotationZ);

    quaternion FinalRotQuat = RotQuatZ*RotQuatY*RotQuatX;

    Result.Basis.xAxis = RotateV3(FinalRotQuat, EntityBasis.Basis.xAxis);
    Result.Basis.yAxis = RotateV3(FinalRotQuat, EntityBasis.Basis.yAxis);
    Result.Basis.zAxis = RotateV3(FinalRotQuat, EntityBasis.Basis.zAxis);
    Result.Offset = RotateV3(FinalRotQuat, EntityBasis.Offset);
    
    Result.Basis.ScaleX = Basis.ScaleX;
    Result.Basis.ScaleY = Basis.ScaleY;
    Result.Basis.ScaleZ = Basis.ScaleZ;

    return(Result);
}

internal render_basis
RotateBasis(render_basis Basis, r32 RotationX, r32 RotationY, r32 RotationZ)
{
    render_basis Result;
    
    quaternion RotQuatX = RotationQuaterion(Basis.xAxis, DegreesToRadians*RotationX);
    quaternion RotQuatY = RotationQuaterion(Basis.yAxis, DegreesToRadians*RotationY);
    quaternion RotQuatZ = RotationQuaterion(Basis.zAxis, DegreesToRadians*RotationZ);

    quaternion FinalRotQuat = RotQuatZ*RotQuatY*RotQuatX;

    Result.xAxis = RotateV3(FinalRotQuat, Basis.xAxis);
    Result.yAxis = RotateV3(FinalRotQuat, Basis.yAxis);
    Result.zAxis = RotateV3(FinalRotQuat, Basis.zAxis);

    Result.ScaleX = Basis.ScaleX;
    Result.ScaleY = Basis.ScaleY;
    Result.ScaleZ = Basis.ScaleZ;

    return(Result);
}

internal render_basis
RotateBasisX(render_basis Basis, r32 RotationX, r32 RotationY, r32 RotationZ)
{
    render_basis Result;
    
    quaternion RotQuatX = RotationQuaterion(Basis.xAxis, DegreesToRadians*RotationX);
    quaternion RotQuatY = RotationQuaterion(Basis.yAxis, DegreesToRadians*RotationY);
    quaternion RotQuatZ = RotationQuaterion(Basis.zAxis, DegreesToRadians*RotationZ);

    quaternion FinalRotQuat = RotQuatZ*RotQuatY*RotQuatX;

    Result.xAxis = RotateV3(FinalRotQuat, Basis.xAxis);
    Result.yAxis = Basis.yAxis;
    Result.zAxis = Basis.zAxis;
    
    Result.ScaleX = Basis.ScaleX;
    Result.ScaleY = Basis.ScaleY;
    Result.ScaleZ = Basis.ScaleZ;

    return(Result);
}

internal render_basis
RotateBasisY(render_basis Basis, r32 RotationX, r32 RotationY, r32 RotationZ)
{
    render_basis Result;
    
    quaternion RotQuatX = RotationQuaterion(Basis.xAxis, DegreesToRadians*RotationX);
    quaternion RotQuatY = RotationQuaterion(Basis.yAxis, DegreesToRadians*RotationY);
    quaternion RotQuatZ = RotationQuaterion(Basis.zAxis, DegreesToRadians*RotationZ);

    quaternion FinalRotQuat = RotQuatZ*RotQuatY*RotQuatX;

    Result.xAxis = Basis.xAxis;
    Result.yAxis = RotateV3(FinalRotQuat, Basis.yAxis);
    Result.zAxis = Basis.zAxis;
    
    Result.ScaleX = Basis.ScaleX;
    Result.ScaleY = Basis.ScaleY;
    Result.ScaleZ = Basis.ScaleZ;

    return(Result);
}
