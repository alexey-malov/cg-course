#pragma once

class IRotationControllerListener
{
public:
	virtual void OnRotationControllerUpdate() = 0;

protected:
	IRotationControllerListener(void){}
	virtual ~IRotationControllerListener(void){}
};

template <class Base>
class CRotationControllerListenerImpl : public Base
{
public:
	virtual void OnRotationControllerUpdate()
	{
	}
protected:
	CRotationControllerListenerImpl(){}
	virtual ~CRotationControllerListenerImpl(){}
};