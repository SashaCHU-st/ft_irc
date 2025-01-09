/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:14:42 by alli              #+#    #+#             */
/*   Updated: 2025/01/07 13:14:45 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

std::vector<std::string> Utils::splitStr(const std::string& str, std::string delim) {
	std::vector<std::string> newList;
	// std::string item;
	int length = str.length();
	size_t start = 0;
	size_t end = 0;
	
	if (length == 0)
		return newList;
	while ((end = str.find(delim, start)) != std::string::npos )
	{
			newList.push_back(str.substr(start, end - start));
			start = end + delim.length();
	}
	newList.push_back(str.substr(start)); // adds the last element from the string
	return newList;
}