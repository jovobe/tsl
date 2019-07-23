namespace tse {

template<typename T>
void resolution<T>::increment() {
    val += 1;
}

template<typename T>
void resolution<T>::decrement() {
    if (val != 1) {
        val -= 1;
    }
}

template<typename T>
T resolution<T>::get() const {
    return val;
}

template<typename T>
void resolution<T>::set(T val) {
    if (val >= 1) {
        this->val = val;
    }
}

template<typename T>
T* resolution<T>::data()
{
    return &val;
}

}
