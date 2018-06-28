// #include <streams++/archive.hpp>

// namespace streams {

// Archive::iterator::iterator(Archive *ptr) : m_obj(ptr) {
//   //   if (ptr == nullptr) {
//   //     m_idx = -1;
//   //   } else {
//   //     m_keys = m_obj->keys();
//   //   }

//   //   if (m_keys.size() > 0) {
//   //     m_current.first = m_keys[0];
//   //     m_current.second = m_obj->get(m_keys[0]);
//   //   }
// }
// Archive::iterator::self_type Archive::iterator::operator++() {
//   //   self_type i = *this;
//   //   m_idx++;
//   //   if (m_idx > m_keys.size()) {
//   //     throw std::runtime_error("overflow");
//   //   } else if (m_idx == m_keys.size()) {
//   //     m_idx = -1;
//   //     return *this;
//   //   }
//   //   auto &key = m_keys[m_idx];
//   //   m_current.first = std::string(key);
//   //   m_current.second = m_obj->get(key);
//   //   return i;
// }
// Archive::iterator::self_type Archive::iterator::operator++(int junk) {

//   //   m_idx++;
//   //   if (m_idx > m_keys.size()) {
//   //     throw std::runtime_error("overflow");
//   //   } else if (m_idx == m_keys.size()) {
//   //     m_idx = -1;
//   //     return *this;
//   //   }
//   //   auto &key = m_keys[m_idx];
//   //   m_current.first = std::string(key);
//   //   m_current.second = m_obj->get(key);
//   //   return *this;
// }
// Archive::iterator::reference Archive::iterator::operator*() {
//   // return m_current;
// }
// Archive::iterator::pointer Archive::iterator::operator->() {
//   // return &m_current;
// }
// bool Archive::iterator::operator==(const Archive::iterator::self_type &rhs) {
//   // return m_idx == rhs.m_idx;
// }
// bool Archive::iterator::operator!=(const Archive::iterator::self_type &rhs) {
//   // return m_idx != rhs.m_idx;
// }

// Archive::Archive() {}

// } // namespace streams