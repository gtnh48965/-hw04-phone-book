#include "phone-book.h"
bool phone_book_t::create_user(const std::string &number, const std::string &name) {
  auto number_iter = prefixes.find(number);
  if (number_iter != prefixes.end()) {
    // проверка что номера уже нет в телефонной книге
    size_t counter = number_iter->second.size();
    ++number_iter;
    while (number_iter != prefixes.end() && number_iter->first.substr(0, number_iter->first.size() - 1) == number) {
      counter -= number_iter->second.size();
      ++number_iter;
    }
    if (counter != 0) {
      return false;
    }
  }
  // вставка
  user_info_t temp;
  temp.user.number = number;
  temp.user.name = name;
  users.insert(temp);
  for (size_t i = 0; i <= number.size(); ++i) {
    prefixes[number.substr(0, i)].insert(temp);
  }
  return true;
}
bool phone_book_t::add_call(const call_t &call) {
  auto number_iter = prefixes.find(call.number);
  if (number_iter == prefixes.end()) {
    // нет такого префикса
    return false;
  }
  auto user_iter = number_iter->second.begin();
  while (user_iter != number_iter->second.end() && user_iter->user.number != call.number) {
    ++user_iter;
  }
  if (user_iter == number_iter->second.end()) {
    // нет такого номера
    return false;
  }
  // удаление
  user_info_t temp = *user_iter;
  users.erase(temp);
  for (size_t i = 0; i <= call.number.size(); ++i) {
    prefixes[call.number.substr(0, i)].erase(temp);
  }
  // вставка
  temp.total_call_duration_s += call.duration_s;
  users.insert(temp);
  for (size_t i = 0; i <= call.number.size(); ++i) {
    prefixes[call.number.substr(0, i)].insert(temp);
  }
  calls.push_back(call);
  return true;
}
std::vector<call_t> phone_book_t::get_calls(size_t start_pos, size_t count) const {
  if (start_pos >= calls.size()) {
    // начало не входит в вектор
    return {};
  }
  if (start_pos + count >= calls.size()) {
    // конец не входит в вектор
    return std::vector<call_t>(calls.begin() + start_pos, calls.end());
  }
  return std::vector<call_t>(calls.begin() + start_pos, calls.begin() + start_pos + count);
}
std::vector<user_info_t> phone_book_t::search_users_by_number(const std::string &number_prefix, size_t count) const {
  auto number_iter = prefixes.find(number_prefix);
  if (number_iter == prefixes.end()) {
    // нет такого префикса
    return {};
  }
  auto user_iter = number_iter->second.begin();
  std::vector<user_info_t> v;
  // вставляем пока выполняются условия
  while (count != 0 && user_iter != number_iter->second.end() && user_iter->user.number.size() >= number_prefix.size() &&
         user_iter->user.number.substr(0, number_prefix.size()) == number_prefix) {
    v.push_back(*user_iter);
    ++user_iter;
    --count;
  }
  return v;
}
std::vector<user_info_t> phone_book_t::search_users_by_name(const std::string &name_prefix, size_t count) const {
  // создаем временного пользователя для поиска
  user_info_t temp;
  temp.user.name = name_prefix;
  temp.total_call_duration_s = std::numeric_limits<double>::max(); // самое большое вещественное число
  auto user_iter = users.lower_bound(temp);
  std::vector<user_info_t> v;
  // вставляем пока выполняются условия
  while (count != 0 && user_iter != users.end() && user_iter->user.name.size() >= name_prefix.size() &&
         user_iter->user.name.substr(0, name_prefix.size()) == name_prefix) {
    v.push_back(*user_iter);
    ++user_iter;
    --count;
  }
  return v;
}
void phone_book_t::clear() {
  users.clear();
  prefixes.clear();
  calls.clear();
}
size_t phone_book_t::size() const {
  return users.size();
}
bool phone_book_t::empty() const {
  return users.empty();
}
bool phone_book_t::compare_name_duration_number::operator()(const user_info_t &left, const user_info_t &right) const {
  if (left.user.name < right.user.name) {
    return true;
  }
  if (left.user.name > right.user.name) {
    return false;
  }
  if (left.total_call_duration_s < right.total_call_duration_s) {
    return false;
  }
  if (left.total_call_duration_s > right.total_call_duration_s) {
    return true;
  }
  return left.user.number < right.user.number;
}
bool phone_book_t::compare_duration_name_number::operator()(const user_info_t &left, const user_info_t &right) const {
  if (left.total_call_duration_s < right.total_call_duration_s) {
    return false;
  }
  if (left.total_call_duration_s > right.total_call_duration_s) {
    return true;
  }
  if (left.user.name < right.user.name) {
    return true;
  }
  if (left.user.name > right.user.name) {
    return false;
  }
  return left.user.number < right.user.number;
}
