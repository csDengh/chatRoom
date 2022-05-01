#pragma once


namespace chatRoom
{
    /**
     * @brief 继承nocopyable类的派生类对象不可被拷贝构造和拷贝赋值
     */
    class nocopyable
    {
    public:
        nocopyable(const nocopyable&) = delete;
        nocopyable operator=(const nocopyable&) = delete;

    protected:
        nocopyable() = default;
        ~nocopyable() = default;
    };

} // namespace chatRoom

