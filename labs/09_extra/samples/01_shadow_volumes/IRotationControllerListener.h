#pragma once

class IRotationControllerListener
{
public:
	virtual void OnRotationControllerUpdate() = 0;

protected:
	IRotationControllerListener(void){}
	~IRotationControllerListener(void){}
};

template <class Base>
class CRotationControllerListenerImpl
{
public:
	virtual void OnRotationControllerUpdate()
	{
	}
protected:
	CRotationControllerListenerImpl(){}
	~CRotationControllerListenerImpl(){}
};