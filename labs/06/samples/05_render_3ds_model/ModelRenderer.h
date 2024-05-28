#pragma once

class CModel;

class CModelRenderer : private boost::noncopyable
{
public:
	CModelRenderer(void);
	void RenderModel(CModel const& model);
};
