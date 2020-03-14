/**
 * @brief 包含模式
 * 模板类的实例化要分成两个步骤，模板的特例化和特例的实例化。
 * 在本文件中，使用显示的模板实例化得到类型
*/

#include"chronography.cpp"
template class Timer<std::chrono::steady_clock>; 
template class Timer<std::chrono::system_clock>; 
// template class Timer<std::chrono::high_resolution_clock>; 