#pragma once

class SolarSystem;

/// Панель управления симуляцией. Читает и правит модель напрямую,
/// поэтому собственного состояния у неё нет — дублировать значения,
/// которые уже есть в модели, незачем.
class ControlPanel
{
public:
    void Render(SolarSystem& system);
};
