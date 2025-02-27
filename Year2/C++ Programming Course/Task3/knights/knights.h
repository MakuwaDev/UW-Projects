#ifndef KNIGHTS_H
#define KNIGHTS_H

#include <limits>
#include <cstddef>
#include <compare>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <list>
#include <utility>

// Represents a single Knight.
class Knight {
public:
    // Maximum possible amount of gold a knight can posses.
    constexpr static size_t MAX_GOLD = std::numeric_limits<size_t>::max();

    constexpr Knight(size_t gold, size_t weapon_class, size_t armour_class)
        : gold_(gold)
        , weapon_class_(weapon_class)
        , armour_class_(armour_class) 
    {}

    // Default copy and move constructors.
    constexpr Knight(Knight const& other) = default;
    constexpr Knight(Knight&& other) = default;

    // Copy assignment operator.
    constexpr Knight& operator=(Knight const& other) = default;

    // Move assignment operator.
    constexpr Knight& operator=(Knight&& other) = default;

    // Accessors for Knight fields.
    constexpr size_t get_gold() const { return gold_; }
    constexpr size_t get_weapon_class() const { return weapon_class_; }
    constexpr size_t get_armour_class() const { return armour_class_; }
    
    // Increase gold by given amount. Caps at MAX_GOLD.
    constexpr void take_gold(size_t gold) { gold_ = (MAX_GOLD - gold > gold_ ? gold_ + gold : MAX_GOLD); }

    // Reset gold amount to zero and return previous value.
    constexpr size_t give_gold() { return std::exchange(gold_, 0); }

    // Set weapon class.
    constexpr void change_weapon(size_t weapon_class) { weapon_class_ = weapon_class; }

    // Reset weapon class to zero and return previous value.
    constexpr size_t give_up_weapon() { return std::exchange(weapon_class_, 0); }

    // Set armour class.
    constexpr void change_armour(size_t armour_class) { armour_class_ = armour_class; }

    // Reset armour class to zero and return previous value.
    constexpr size_t take_off_armour() { return std::exchange(armour_class_, 0); }

    // Take all of the other knight's gold and only the better 
    // (in terms of class) components of his equipment.
    constexpr Knight& operator+=(Knight& other);
    constexpr Knight& operator+=(Knight&& other);

    // Create a new knight with as much gold as "added" knights together,
    // but a maximum of MAX_GOLD and better components of their equipment.
    constexpr Knight const operator+(Knight const& other) const; 

    // Comparison operators based on weapon and armour effectiveness.
    constexpr std::weak_ordering operator<=>(Knight const& other) const;
    constexpr bool operator==(Knight const& other) const {
        return (*this <=> other) == std::weak_ordering::equivalent;
    }

private:
    size_t gold_;           // Amount of gold the knight possesses.
    size_t weapon_class_;   // Strength of the knight's weapon.
    size_t armour_class_;   // Strength of the knight's armour.
};

// Default knight for initialization when no other is provided.
constexpr Knight TRAINEE_KNIGHT(0, 1, 1);

// Represents a tournament of Knights.
class Tournament {
public:
    // Delete the default constructor.
    Tournament() = delete;

    // Initialize a tournament with a list of knights.
    Tournament(const std::initializer_list<Knight>& pretenders) : pretenders_(pretenders) {
        // If no knights provided, add a default TRAINEE_KNIGHT.
        if (pretenders.size() == 0)
            pretenders_.push_back(TRAINEE_KNIGHT);
    }

    // Default copy and move constructors.
    Tournament(const Tournament& other) = default;
    Tournament(Tournament&& other) = default;

    // Copy assignment operator.
    Tournament& operator=(const Tournament& other) = default;

    // Move assignment operator.
    Tournament& operator=(Tournament&& other) = default;

    // Add a knight to a tournament and clear eliminated list.
    Tournament& operator+=(Knight const& knight);
    // Remove knights equal (on all fields) to the parameter and clear eliminated list.
    Tournament& operator-=(Knight const& knight);

    // Simulate the tournament and return the iterator to the winner.
    std::list<Knight>::const_iterator play();

    // Return an iterator to the "no winner" state (end of the list).
    std::list<Knight>::const_iterator no_winner() const { return pretenders_.cend(); }

    // Get total number of knights participating in the tournament.
    size_t size() const { return pretenders_.size() + eliminated_.size(); }
    
    // Overload stream operator to print the tournament's state.
    friend std::ostream& operator<<(std::ostream& os, const Tournament& tournament);

private:
    std::list<Knight> pretenders_;  // Knights still competing.
    std::list<Knight> eliminated_;  // Knights who have been eliminated.
};

// Implementation of Knight methods and its stream output.

constexpr Knight& Knight::operator+=(Knight& other) {
    // Transfer gold and upgrade weapon/armour if the other knight's are better.
    take_gold(other.give_gold());
    if (weapon_class_ < other.weapon_class_)
        change_weapon(other.give_up_weapon());
    if (armour_class_ < other.armour_class_)
        change_armour(other.take_off_armour());
    return *this;
}

constexpr Knight& Knight::operator+=(Knight&& other) {
    // Same logic for rvalue Knights.
    return operator+=(other);
}

constexpr Knight const Knight::operator+(Knight const& other) const {
    // Create copies of the two knights and merge them.
    Knight a(*this), b(other);
    return a += b;
}

constexpr std::weak_ordering Knight::operator<=>(Knight const& other) const {
    // Compare knights based on weapon and armour effectiveness.
    if (weapon_class_ > other.armour_class_ && armour_class_ >= other.weapon_class_)
        return std::weak_ordering::greater; // This knight wins.

    if (other.weapon_class_ > armour_class_ && other.armour_class_ >= weapon_class_)
        return std::weak_ordering::less;    // The other knight wins.

    // Both knights have weapons stronger than their opponent's armor.
    if (weapon_class_ > other.armour_class_ && other.weapon_class_ > armour_class_) {
        if (armour_class_ != other.armour_class_)
            return armour_class_ <=> other.armour_class_;   // Better armour wins.

        return weapon_class_ <=> other.weapon_class_;   // When armor is equal, better weapon wins.
    }

    return std::weak_ordering::equivalent;  // Tie.
}

// Stream output for a Knight.
std::ostream& operator<<(std::ostream& os, Knight const& knight) {
    os << "("
       << knight.get_gold() << " gold, "
       << knight.get_weapon_class() << " weapon class, "
       << knight.get_armour_class() << " armour class"
       << ")\n";
    return os;
}

// Implementation of Tournament methods and friends.

Tournament& Tournament::operator+=(Knight const& knight) {
    // Clear eliminated list and add a knight to the tournament.
    eliminated_.clear();
    pretenders_.push_back(knight);
    return *this;
}

Tournament& Tournament::operator-=(Knight const& knight) {
    eliminated_.clear();

    // Lambda function for equality of two knights on all fields.
    auto equal = [&knight](Knight const& k) {
        return k.get_gold() == knight.get_gold() &&
               k.get_armour_class() == knight.get_armour_class() &&
               k.get_weapon_class() == knight.get_weapon_class();
    };

    std::erase_if(pretenders_, equal);
    return *this;
}

std::list<Knight>::const_iterator Tournament::play() {
    eliminated_.clear();

    while (pretenders_.size() >= 2) {
        // Get first two knights.
        Knight knight1 =pretenders_.front();
        pretenders_.pop_front();
        Knight knight2 = pretenders_.front();
        pretenders_.pop_front();
        
        if (knight1 == knight2) {
            // If knights tie, move both to eliminated list.
            eliminated_.push_front(knight2);
            eliminated_.push_front(knight1);
        }
        else {
            // Determine the winner/loser.
            Knight& winner = knight1 > knight2 ? knight1 : knight2;
            Knight& loser = knight1 < knight2 ? knight1 : knight2;

            // Winner takes losers equipment.
            winner += loser;

            // Move knights to adequate places.
            pretenders_.push_back(winner);
            eliminated_.push_front(loser);
        }
    }

    // If a knight remains he is the winner. Otherwise there is no winner.
    return pretenders_.empty() ? pretenders_.cend() : pretenders_.cbegin();
}

// Tournaments stream output: pretenders with a '+' prefix,
// eliminated with a '-' prefix and finally an '='.
std::ostream& operator<<(std::ostream& os, const Tournament& tournament) {
    for (const auto& knight : tournament.pretenders_)
        os << "+ " << knight;
    
    for (const auto& knight : tournament.eliminated_)
        os << "- " << knight;
    
    os << "=\n";

    return os;
}

// Compile-time only function to return a pair of size_ts representing weapon class and armour class
// of a knight whose equipment classes differ the most. 
consteval std::pair<size_t, size_t> max_diff_classes(std::initializer_list<Knight> knights) {
    // Lambda function for absolute value of a difference, since std::abs isn't constexpr.
    auto abs_diff = [](size_t a, size_t b) { return a > b ? a - b : b - a; };

    // Lambda function to compare two knights by the difference between their armour and weapon class.
    auto compare_diff = [&abs_diff](Knight const& a, Knight const& b) {
        return abs_diff(a.get_armour_class(), a.get_weapon_class()) < 
               abs_diff(b.get_armour_class(), b.get_weapon_class());
    };

    auto max_knight = std::max(knights, compare_diff);

    return std::make_pair(max_knight.get_weapon_class(), max_knight.get_armour_class());
}

#endif
