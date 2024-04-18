#pragma once

class IRotationControllerListener
{
public:
	virtual void OnRotationControllerUpdate() = 0;

protected:
	IRotationControllerListener() = default;
	virtual ~IRotationControllerListener() = default;
};

template <class Base>
class CRotationControllerListenerImpl : public Base
{
public:
	virtual void OnRotationControllerUpdate() {}
};
