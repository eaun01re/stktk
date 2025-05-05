#pragma once


#include <functional>

#include <SFML/Graphics.hpp>

#include <game/objects/object.h>


class Box final : public Object
{
public:
    enum class Direction
    {
        None,
        Left,
        Right,
        Down
    };
    using MoveStartedCallback = std::function<void(Id)>;
    using MoveFinishedCallback = std::function<void(Id)>;

public:
    explicit Box(
        const MoveStartedCallback &moveStartedCallback,
        const MoveFinishedCallback &moveFinishedCallback);
    virtual ~Box() = default;

    void update(const Duration &elapsed) override;
    void move(Direction direction);
    bool blow();
    bool isBlowing() const noexcept;
    bool isBlowed() const noexcept;

protected:
    void moveStarted() override;
    void moveFinished() override;

private:
    void init();

private:
    std::optional<Duration> m_blowDuration;
    MoveStartedCallback m_moveStartedCallback;
    MoveFinishedCallback m_moveFinishedCallback;
};


using BoxPtr = std::shared_ptr<Box>;
