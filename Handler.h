#pragma once
#include <string>
#include <memory>

class GameFacade;

class Handler {
protected:
    std::shared_ptr<Handler> next_;
    GameFacade* facade_;

    bool passToNext(const std::string& cmd) const {
        return next_ ? next_->handle(cmd) : false;
    }

public:
    explicit Handler(GameFacade* facade) : facade_(facade) {}
    virtual ~Handler() = default;

    void setNext(std::shared_ptr<Handler> next) {
        next_ = std::move(next);
    }

    virtual bool handle(const std::string& cmd) = 0;
};

class CreateHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class MoveHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class AttackHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class CollectHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class HealHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class BuildHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class ResearchHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class DistributeHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class ProduceHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class StatsHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};
