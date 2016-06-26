#ifndef __STD_REGEX_EX_H__
#define __STD_REGEX_EX_H__

#include <regex>

// 扩展regex_replace，使其支持回调
namespace std
{
  using ReplaceFunc_t = std::function<string(const smatch&)>;

  template<class BidirIt, class Traits, class CharT>
  std::basic_string<CharT> regex_replace(BidirIt first, BidirIt last,
    const std::basic_regex<CharT, Traits>& re, ReplaceFunc_t f)
  {
    std::basic_string<CharT> s;

    typename std::match_results<BidirIt>::difference_type
      positionOfLastMatch = 0;
    auto endOfLastMatch = first;

    auto callback = [&](const std::match_results<BidirIt>& match)
    {
      auto positionOfThisMatch = match.position(0);
      auto diff = positionOfThisMatch - positionOfLastMatch;

      auto startOfThisMatch = endOfLastMatch;
      std::advance(startOfThisMatch, diff);

      s.append(endOfLastMatch, startOfThisMatch);
      s.append(f(match));

      auto lengthOfMatch = match.length(0);

      positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

      endOfLastMatch = startOfThisMatch;
      std::advance(endOfLastMatch, lengthOfMatch);
    };

    std::sregex_iterator begin(first, last, re), end;
    std::for_each(begin, end, callback);

    s.append(endOfLastMatch, last);

    return s;
  }

  template<class Traits, class CharT>
  std::string regex_replace(const std::string& s,
    const std::basic_regex<CharT, Traits>& re, ReplaceFunc_t f)
  {
    return regex_replace(s.cbegin(), s.cend(), re, f);
  }
} // namespace std

#endif // !__STD_REGEX_EX_H__
