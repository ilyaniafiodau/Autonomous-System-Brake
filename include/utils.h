namespace utils
{

template <typename T>
T convert(T value, T From1, T From2, T To1, T To2)
{
  return ((value - From1) / (From2 - From1) * (To2 - To1) + To1);
}

}  // namespace utils
