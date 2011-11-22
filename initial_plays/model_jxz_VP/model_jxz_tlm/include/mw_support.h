#ifndef __MW_SUPPORT_H__
#define __MW_SUPPORT_H__
#include <systemc>
#include <map>

template <typename T>
  class mw_comlink_fifo : public sc_core::sc_object
{
 public:
  mw_comlink_fifo(const char* name, unsigned int size) : sc_core::sc_object(name),
    m_size(size)
  {
    clear();
    if (m_size > 0 ) {
      m_fifo = new T[m_size];
    }
  }

  ~mw_comlink_fifo()
  {
    clear();
    delete [] m_fifo;
  }

  void clear()
  {
    m_free = m_size;
    m_readindex = 0;
    m_writeindex = 0;
  }

  bool nb_read(T* data)
  {
    if (m_free == m_size ) {
      return false;
    }

    *data = m_fifo[m_readindex];
    m_readindex = ( m_readindex + 1 ) % m_size;
    m_free++;
    return true;
  }

  bool nb_write(const T* data)
  {
    if (m_free == 0 ) {
      return false;
    }

    m_fifo[m_writeindex] = *data;
    m_writeindex = ( m_writeindex + 1 ) % m_size;
    m_free--;
    return true;
  }

  unsigned int get_num_available() const
  {
    return m_size-m_free;
  }

  unsigned int get_num_free() const
  {
    return m_free;
  }

 private:
  T* m_fifo;
  unsigned int m_size;
  unsigned int m_readindex;
  unsigned int m_writeindex;
  unsigned int m_free;
};                                     //mw_comlink_fifo

template <typename T>
  class mw_comlink_timedreg : public sc_core::sc_object
{
 public:
  mw_comlink_timedreg(const char* name) : sc_core::sc_object(name)
  {
    clear();
  }

  ~mw_comlink_timedreg()
  {
    clear();
  }

  typedef std::pair<const sc_core::sc_time, T> pair_type;
  typedef typename std::multimap<const sc_core::sc_time,T>::iterator it_type;
  void clear()
  {
    m_timedreg.clear();
  }

  void clear(const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    m_timedreg.erase(m_timedreg.begin(),m_timedreg.lower_bound(globaltime));
  }

  bool nb_read(T* data,const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    it_type it_first_future = m_timedreg.upper_bound(globaltime);
    it_type it_lastest_past = it_first_future;//first item in the future;
    if (it_first_future == m_timedreg.begin() ) {
      return false;
    }

    it_lastest_past--;                 //init to the lastest item in the past;
    *data = (*it_lastest_past).second;

    //erase everything older than actual time (register behavior)
    m_timedreg.erase(m_timedreg.begin(),it_first_future);
    return true;
  }

  bool nb_write(const T* data,const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    m_timedreg.insert(pair_type(globaltime,*data));
    return true;
  }

 private:
  std::multimap< const sc_core::sc_time,T > m_timedreg;
};                                     //mw_comlink_timedreg

template <typename T>
  class mw_comlink_timedfifo : public sc_core::sc_object
{
 public:
  mw_comlink_timedfifo(const char* name, unsigned int size) : sc_core::sc_object
    (name), m_size(size)
  {
    clear();
  }

  ~mw_comlink_timedfifo()
  {
    clear();
  }

  typedef std::pair<const sc_core::sc_time, T> pair_type;
  typedef typename std::multimap<const sc_core::sc_time,T>::iterator it_type;
  void clear()
  {
    m_timedfifo.clear();
  }

  void clear(const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    m_timedfifo.erase(m_timedfifo.begin(),m_timedfifo.lower_bound(globaltime));
  }

  bool nb_read(T* data,const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    it_type it = m_timedfifo.upper_bound(globaltime);
    unsigned int nb_elem = 0;
    while ((it != m_timedfifo.begin()) && (nb_elem < m_size)) {
      it--;
      nb_elem++;
    }

    if (nb_elem == 0) {
      return false;
    }

    *data = (*it).second;
    it++;                              //move to next item (new head) and erase everything older;
    m_timedfifo.erase(m_timedfifo.begin(),it);
    return true;
  }

  bool nb_write(const T* data,const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    it_type it = m_timedfifo.upper_bound(globaltime);
    unsigned int nb_elem = 0;
    while ((it != m_timedfifo.begin())&& (nb_elem < m_size)) {
      it--;
      nb_elem++;
    }

    if (nb_elem >= m_size) {
      return false;
    }

    m_timedfifo.insert(pair_type(globaltime,*data));
    return true;
  }

  unsigned int get_num_available(const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    it_type it = m_timedfifo.upper_bound(globaltime);
    unsigned int nb_elem = 0;
    while ((it != m_timedfifo.begin())&& (nb_elem < m_size)) {
      it--;
      nb_elem++;
    }

    return nb_elem;
  }

  unsigned int get_num_free(const sc_core::sc_time& localtime)
  {
    sc_core::sc_time globaltime = sc_core::sc_time_stamp() + localtime;
    it_type it = m_timedfifo.upper_bound(globaltime);
    unsigned int nb_elem = 0;
    while ((it != m_timedfifo.begin())&& (nb_elem < m_size)) {
      it--;
      nb_elem++;
    }

    return m_size-nb_elem;
  }

 private:
  std::multimap< const sc_core::sc_time,T > m_timedfifo;
  unsigned int m_size;
};                                     //mw_comlink_timedfifo

class mw_refresh_reg
{
 public:
  mw_refresh_reg(unsigned long nb_reg)
  {
    m_max = ~((~0ULL) << nb_reg);
  }

  ~mw_refresh_reg()
  {
    ;
  }

  inline void init(bool full)
  {
    m_reg = full? m_max: 0ULL;
  }

  inline void set(unsigned long pos)
  {
    m_reg |= (1ULL << pos);
  }

  inline bool test(void)
  {
    return (m_reg == m_max);
  }

 private:
  unsigned long long m_reg;
  unsigned long long m_max;
};                                     //mw_refresh_reg

#endif                                 //__MW_SUPPORT_H__
