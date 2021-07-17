#pragma once

class ApplicationState {
public:
	ApplicationState() noexcept = default;
	ApplicationState(const ApplicationState&) = delete;
	virtual ~ApplicationState() = default;

public:
	ApplicationState& operator=(const ApplicationState&) = delete;

public:
	virtual bool Initialize() = 0;
	virtual int Run() = 0;
	virtual void Finalize() = 0;
};