// <copyright file="PatternDocumentationHelper.cs" company="None">
// Copyright (c) None. All rights reserved.
// </copyright>

namespace GameOffsets
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    /// <summary>
    ///     Helper class to generate documentation and information about patterns and offsets.
    ///     This class is primarily for development and documentation purposes.
    /// </summary>
    public static class PatternDocumentationHelper
    {
        /// <summary>
        ///     Gets detailed information about all available patterns.
        /// </summary>
        /// <returns>List of pattern information.</returns>
        public static List<PatternInfo> GetAllPatternInfo()
        {
            var patterns = StaticOffsetsPatterns.Patterns;
            var patternInfoList = new List<PatternInfo>();

            foreach (var pattern in patterns)
            {
                patternInfoList.Add(new PatternInfo
                {
                    Name = pattern.Name,
                    PatternString = GetPatternString(pattern),
                    BytesToSkip = pattern.BytesToSkip,
                    DataLength = pattern.Data.Length,
                    MaskLength = pattern.Mask.Length,
                    WildcardCount = pattern.Mask.Count(m => !m),
                    Description = GetPatternDescription(pattern.Name)
                });
            }

            return patternInfoList;
        }

        /// <summary>
        ///     Generates a markdown table of all patterns.
        /// </summary>
        /// <returns>Markdown formatted table string.</returns>
        public static string GeneratePatternTable()
        {
            var patterns = GetAllPatternInfo();
            var sb = new StringBuilder();

            sb.AppendLine("| Pattern Name | Data Length | Wildcards | Bytes to Skip | Description |");
            sb.AppendLine("|--------------|-------------|-----------|---------------|-------------|");

            foreach (var pattern in patterns)
            {
                sb.AppendLine($"| {pattern.Name} | {pattern.DataLength} | {pattern.WildcardCount} | {pattern.BytesToSkip} | {pattern.Description} |");
            }

            return sb.ToString();
        }

        /// <summary>
        ///     Gets a human-readable string representation of a pattern.
        /// </summary>
        /// <param name="pattern">The pattern to convert.</param>
        /// <returns>Human-readable pattern string.</returns>
        private static string GetPatternString(Pattern pattern)
        {
            var sb = new StringBuilder();
            var skipIndex = pattern.BytesToSkip;

            for (var i = 0; i < pattern.Data.Length; i++)
            {
                if (i == skipIndex)
                {
                    sb.Append("^ ");
                }

                if (pattern.Mask[i])
                {
                    sb.Append($"{pattern.Data[i]:X2} ");
                }
                else
                {
                    sb.Append("?? ");
                }
            }

            return sb.ToString().Trim();
        }

        /// <summary>
        ///     Gets a description for a pattern based on its name.
        /// </summary>
        /// <param name="patternName">Name of the pattern.</param>
        /// <returns>Description of what the pattern finds.</returns>
        private static string GetPatternDescription(string patternName)
        {
            return patternName switch
            {
                "Game States" => "Main game state structure with all game states (login, character select, in-game, etc.)",
                "File Root" => "Root of the file loading system for accessing loaded game files",
                "AreaChangeCounter" => "Counter that increments when player changes areas/zones",
                "GameWindowScaleValues" => "UI scaling values for proper overlay positioning",
                "Terrain Rotator Helper" => "Helper data for terrain rotation calculations",
                "Terrain Rotation Selector" => "Additional terrain rotation data for map positioning",
                _ => "Custom pattern - see code comments for details"
            };
        }

        /// <summary>
        ///     Validates all patterns for potential issues.
        /// </summary>
        /// <returns>List of validation issues found.</returns>
        public static List<string> ValidatePatterns()
        {
            var issues = new List<string>();
            var patterns = StaticOffsetsPatterns.Patterns;

            // Check for duplicate names
            var names = patterns.Select(p => p.Name).ToList();
            var duplicateNames = names.GroupBy(n => n).Where(g => g.Count() > 1).Select(g => g.Key);
            foreach (var duplicateName in duplicateNames)
            {
                issues.Add($"Duplicate pattern name found: {duplicateName}");
            }

            // Check for patterns that might be too short (less reliable)
            foreach (var pattern in patterns)
            {
                if (pattern.Data.Length < 8)
                {
                    issues.Add($"Pattern '{pattern.Name}' is very short ({pattern.Data.Length} bytes) - may not be unique enough");
                }

                var wildcardPercent = (double)pattern.Mask.Count(m => !m) / pattern.Data.Length * 100;
                if (wildcardPercent > 50)
                {
                    issues.Add($"Pattern '{pattern.Name}' has high wildcard percentage ({wildcardPercent:F1}%) - may not be unique enough");
                }

                if (pattern.BytesToSkip < 0 || pattern.BytesToSkip >= pattern.Data.Length)
                {
                    issues.Add($"Pattern '{pattern.Name}' has invalid BytesToSkip value ({pattern.BytesToSkip})");
                }
            }

            return issues;
        }

        /// <summary>
        ///     Prints pattern information to console for debugging.
        /// </summary>
        public static void PrintPatternInfo()
        {
            var patterns = GetAllPatternInfo();
            
            Console.WriteLine("=== GameOverlay Pattern Information ===");
            Console.WriteLine($"Total Patterns: {patterns.Count}");
            Console.WriteLine();

            foreach (var pattern in patterns)
            {
                Console.WriteLine($"Name: {pattern.Name}");
                Console.WriteLine($"  Length: {pattern.DataLength} bytes");
                Console.WriteLine($"  Wildcards: {pattern.WildcardCount}");
                Console.WriteLine($"  Bytes to Skip: {pattern.BytesToSkip}");
                Console.WriteLine($"  Description: {pattern.Description}");
                Console.WriteLine();
            }

            var issues = ValidatePatterns();
            if (issues.Any())
            {
                Console.WriteLine("=== Pattern Validation Issues ===");
                foreach (var issue in issues)
                {
                    Console.WriteLine($"WARNING: {issue}");
                }
            }
            else
            {
                Console.WriteLine("=== All Patterns Validated Successfully ===");
            }
        }
    }

    /// <summary>
    ///     Information about a pattern for documentation purposes.
    /// </summary>
    public class PatternInfo
    {
        /// <summary>
        ///     Gets or sets the name of the pattern.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        ///     Gets or sets the pattern string representation.
        /// </summary>
        public string PatternString { get; set; }

        /// <summary>
        ///     Gets or sets the number of bytes to skip.
        /// </summary>
        public int BytesToSkip { get; set; }

        /// <summary>
        ///     Gets or sets the length of the pattern data.
        /// </summary>
        public int DataLength { get; set; }

        /// <summary>
        ///     Gets or sets the length of the pattern mask.
        /// </summary>
        public int MaskLength { get; set; }

        /// <summary>
        ///     Gets or sets the number of wildcard bytes in the pattern.
        /// </summary>
        public int WildcardCount { get; set; }

        /// <summary>
        ///     Gets or sets the description of what the pattern finds.
        /// </summary>
        public string Description { get; set; }
    }
}