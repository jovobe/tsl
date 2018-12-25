namespace tsl {

template<typename T>
void resolution<T>::increment() {
    data += 1;
}

template<typename T>
void resolution<T>::decrement() {
    if (data != 1) {
        data -= 1;
    }
}

template<typename T>
T resolution<T>::get() const {
    return data;
}

template<typename T>
void resolution<T>::set(T val) {
    if (val >= 1) {
        data = val;
    }
}

}
