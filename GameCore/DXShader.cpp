#include "DXShader.h"

bool DXShader::SetInputLayout()
{
	m_pImmediateContext->IASetInputLayout(InputLayout);

	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	m_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);

	return true;
}

bool DXShader::SetShader()
{
	m_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	m_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	m_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	m_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	m_pImmediateContext->PSSetShader(PixelShader, NULL, 0);

	return true;
}

bool DXShader::SetState()
{
	m_pImmediateContext->RSSetState(RSState);
	m_pImmediateContext->OMSetBlendState(BlendState, 0, -1);
	m_pImmediateContext->PSSetSamplers(0, 1, &SamplerState);
	return false;
}
